/**
 * @file BitwiseBufferedFile.c
 * @author Cosimo Sacco <cosimosacco@gmail.com>
 * @author Davide Silvestri <davidesil.web@gmail.com>
 *
 * @section LICENSE
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include "../utilities/mathUtils.h"
#include "BitwiseBufferedFile.h"

struct BitwiseBufferedFile
{
    int fileDescriptor;
    int mode;
    CELL_TYPE buffer[BUFFER_CELLS];
    int position;
    int availableBits;
    int emptyFile;
};

struct BitwiseBufferedFile* openBitwiseBufferedFile
(
    const char* pathToFile,
    int mode,
    int fileDescriptorToSet,
    FILE* fileToSet
)
{
    int fileDescriptor = -1;
    struct BitwiseBufferedFile* bitFile = NULL;
    if(mode != O_RDONLY && mode != O_WRONLY)
    {
        errno = EINVAL; // Invalid parameter
        return NULL;
    }
    bitFile = calloc(1, sizeof(struct BitwiseBufferedFile));
    if(bitFile == NULL)
    {
        errno = ENOMEM; // Memory allocation failed
        return NULL;
    }
    fileDescriptor =
        (pathToFile != NULL)?
            open(pathToFile, (mode == O_RDONLY)? O_RDONLY : (O_WRONLY | O_CREAT | O_TRUNC), 0644) :
        (fileDescriptorToSet != -1)?
            fileDescriptor :
        (fileToSet != NULL)?
            fileno(fileToSet) : -1;
    if(fileDescriptor < 0) // File neither found nor created
    {
        free(bitFile);
        return NULL;
    }
    bitFile->fileDescriptor = fileDescriptor;
    bitFile->mode = mode;
    return bitFile;
}

ssize_t storeBitBuffer(int fileDescriptor, CELL_TYPE* buffer, size_t count)
{
    ssize_t n = count/sizeof(CELL_TYPE) + (count%sizeof(CELL_TYPE) > 0);
    ssize_t writtenBytes = 0;
    uint8_t* byteBuffer = (uint8_t*)buffer; // buffer seen as a byte buffer
    if
    (
        buffer == NULL       ||
        fileDescriptor == -1 ||
        count > BUFFER_BYTES ||
        count < 0
    )
    {
        errno = EINVAL;
        return -1;
    }
    #if CELL_TYPE_LENGTH != 8
        /**
         * Due to portability reasons, a standard serialization format must be
         * used. The cost of the following conversion is O(BUFFER_CELLS), but it
         * happens only when the buffer is full -> amortized cost: O(1)
         **/
        for(; n--;) buffer[n] = HOST_TO_LITTLE_ENDIAN_CONVERT(buffer[n]);
    #endif
    n = 0; // Now n == 0
    while(n < count) // n indexes bytes now
    {
        writtenBytes = write(fileDescriptor, byteBuffer + n, count - n);
        if(writtenBytes == -1) return -1;
        n += writtenBytes;
    }
    return n;
}

ssize_t loadBitBuffer(int fileDescriptor, CELL_TYPE* buffer, size_t count)
{
    ssize_t n = 0;
    ssize_t readBytes = 0;
    uint8_t* byteBuffer = (uint8_t*)buffer;
    if
    (
        byteBuffer == NULL   ||
        fileDescriptor == -1 ||
        count < 0            ||
        count > BUFFER_BYTES
    )
    {
        errno = EINVAL;
        return -1;
    }
    while(n < count)
    {
        readBytes = read(fileDescriptor, byteBuffer + n, count - n);
        if(readBytes == -1) return -1;
        n += readBytes;
        if(readBytes == 0) break;
    }
    readBytes = n;
    #if CELL_TYPE_LENGTH != 8
        int offset = (n%sizeof(CELL_TYPE)); //era & ((ssize_t)(MODULO_MASK))
        n = n/(sizeof(CELL_TYPE));// era n >>= SHIFT_FACTOR;
        if(offset != 0) //if(n % sizeof(CELL_TYPE) != 0)
        {
            /*TODO gestire il caso in cui vengano letti un numero di byte non
            congruente con la dimensione della cella*/
            n++;
            buffer[n] &= ((((CELL_TYPE)1) << offset*8) - 1);
        }
        // n refers to the number of read cells
        /**
         * Due to portability reasons, a standard serialization format must be
         * used. The cost of the following conversion is O(BUFFER_CELLS), but it
         * happens only when the buffer is full -> amortized cost: O(1)
         **/
        for(; n--;) buffer[n] = LITTLE_ENDIAN_TO_HOST_CONVERT(buffer[n]);
    #endif
    return readBytes;
}

int closeBitwiseBufferedFile(struct BitwiseBufferedFile* bitFile)
{
    int error;
    if(bitFile == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    if(bitFile->mode == O_WRONLY && bitFile->availableBits != 0)
    {
        int index = bitFile->position/(sizeof(CELL_TYPE)*8); //era  >> BITWISE_SHIFT_FACTOR
        int offset = bitFile->position%(sizeof(CELL_TYPE)*8); //era & BITWISE_MODULO_MASK
        if(offset)
        {
            bitFile->buffer[index] &= ((CELL_TYPE)1 << offset) - 1;
        }
        storeBitBuffer
        (
            bitFile->fileDescriptor,
            bitFile->buffer,
            index*sizeof(CELL_TYPE) + (offset/8) + (offset > 0)
        );
    }
    error = close(bitFile->fileDescriptor);
    memset(bitFile, 0, sizeof(struct BitwiseBufferedFile));
    free(bitFile);
    return error;
}

ssize_t readBitBuffer
(
    struct BitwiseBufferedFile* bitFile, CELL_TYPE* data, size_t length
)
{
    int err;
    int bitsToBeRead;
    int readBits = 0;
    int index;
    int offset;
    CELL_TYPE mask;
    if
    (
        data == NULL              ||
        bitFile == NULL           ||
        length > CELL_TYPE_LENGTH ||
        length < 0
    )
    {
        errno = EINVAL;
        return -1;
    }
    if(bitFile->mode != O_RDONLY)
    {
        errno = EBADF;
        return -1;
    }
    if(bitFile->emptyFile) return 0;
    while(length > 0)
    {
        if
        (
            bitFile->position >= BUFFER_CELLS*CELL_TYPE_LENGTH ||
            bitFile->availableBits == 0
        )
        {
            err =
                loadBitBuffer
                (
                    bitFile->fileDescriptor, bitFile->buffer, BUFFER_BYTES
                );
            if(err == -1) return -1;
            if(err == 0)
            {
                bitFile->emptyFile = 1;
                return readBits;
            }
            bitFile->position = 0;
            bitFile->availableBits = err*8;
        }
        index = (bitFile->position)/(sizeof(CELL_TYPE)*8); //era  >> BITWISE_SHIFT_FACTOR
        offset = (bitFile->position)%(sizeof(CELL_TYPE)*8); //era & BITWISE_MODULO_MASK
        bitsToBeRead =
            min
            (
                CELL_TYPE_LENGTH - offset, min(length, bitFile->availableBits)
            );
        mask =
            (bitsToBeRead == CELL_TYPE_LENGTH)?
                FULL_MASK :
                (((((CELL_TYPE)1) << bitsToBeRead) - 1) << offset);
        *data &= ~((((CELL_TYPE)1 << bitsToBeRead) - 1) << readBits);
        *data |= (offset - readBits > 0)?
            (bitFile->buffer[index] & mask) >> (offset - readBits) :
            (bitFile->buffer[index] & mask) << (readBits - offset);
        length -= bitsToBeRead;
        bitFile->availableBits -= bitsToBeRead;
        bitFile->position += bitsToBeRead;
        readBits += bitsToBeRead;
    }
    //printf("Dal buffer è stato letto: %u su %i", *data, readBits);
    return readBits;
}

ssize_t writeBitBuffer
(
    struct BitwiseBufferedFile* bitFile, CELL_TYPE data, size_t length
)
{
   // printf("Nel buffer è stato scritto: %i\n\n",data);
    int index;
    int offset;
    int bitsToBeWritten;
    CELL_TYPE mask;
    if(bitFile == NULL || length < 0 || length > CELL_TYPE_LENGTH)
    {
        errno = EINVAL;
        return -1;
    }
    if(bitFile->mode != O_WRONLY)
    {
        errno = EBADF;
        return -1;
    }
    while(length > 0)
    {
        index = (bitFile->position)/(sizeof(CELL_TYPE)*8); //era & BITWISE_SHIFT_FACTOR
        offset = (bitFile->position)%(sizeof(CELL_TYPE)*8); //era & BITWISE_MODULO_MASK
        bitsToBeWritten = min(CELL_TYPE_LENGTH - offset, length);
        /* The following check addresses the rotate shift problem */
        mask =
            (bitsToBeWritten == CELL_TYPE_LENGTH)?
            FULL_MASK : ((((CELL_TYPE)1) << bitsToBeWritten) - 1);
        bitFile->buffer[index] &= (((CELL_TYPE)1) << offset) - 1;
        bitFile->buffer[index] |= (data & mask) << offset;
        length -= bitsToBeWritten;
        bitFile->position += bitsToBeWritten;
        bitFile->availableBits += bitsToBeWritten;
        if(bitFile->position >= BUFFER_CELLS*CELL_TYPE_LENGTH)
        {
            if
            (
                storeBitBuffer
                (
                    bitFile->fileDescriptor,
                    bitFile->buffer,
                    BUFFER_BYTES
                ) == -1
            ) return -1;
            bitFile->position = 0;
        }
        data >>= bitsToBeWritten;
    }
    return 0;
}

inline int emptyFile(struct BitwiseBufferedFile* bitFile)
{
    return bitFile->emptyFile;
}
