/**
 * @file LZ78Compressor.c
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

#include "LZ78Compressor.h"
#include "LZ78HashTable.h"
#include "Commons.h"
#include "BitwiseBufferedFile.h"
#include <errno.h>

/**
 * The bytes of the file to be compressed are read using fread.
 * fread improves the performances of the read syscall adding a buffering
 * layer: the cost of the read syscall is amortized reading more than the
 * requested bytes and buffering them, so that the next calls can avoid the
 * costly read syscall and return the buffer content.
 * Anyway, fread also has a cost (at least, the cost of construction of the
 * activation record).
 * The compressor choises are taken for each byte read. But, if a different
 * call to fread is done for each read byte, the total cost of calling fread
 * can be high. To reduce that cost, we adopted the same strategy used by
 * fread: another buffering layer is introduced. The bytes are accessed on
 * the buffer, and when all the buffered bytes have been used, a fread call
 * is done to fill up the buffer. So, the fread cost is amortized of a
 * factor which is proportional to the buffer size.
 * The LOCAL_BYTE_BUFFER_LENGTH constant is the length for this local buffer.
 **/
#define LOCAL_BYTE_BUFFER_LENGTH 128

int compress(FILE* inputFile, FILE* outputFile, int compressionLevel)
{
    struct BitwiseBufferedFile* w = openBitwiseBufferedFile(NULL, 1, -1, outputFile);
    uint8_t readByte[LOCAL_BYTE_BUFFER_LENGTH];
    size_t indexLength = INITIAL_INDEX_LENGTH;
    size_t bufferedBytes;
    int byteIndex = 0;
    struct LZ78HashTableEntry* hashTable;
    uint32_t childIndex = 257;
    uint32_t lookupIndex = ROOT_INDEX;
    uint32_t indexLengthMask = (1 << INITIAL_INDEX_LENGTH) - 1;
    uint32_t child;
    if((compressionLevel < MIN_COMPRESSION_LEVEL || compressionLevel > MAX_COMPRESSION_LEVEL) || inputFile == NULL || w == NULL)
    {
        errno = EINVAL;
        if(w != NULL) closeBitwiseBufferedFile(w);
        return -1;
    }
    uint32_t hashTableEntries = compressionParameters[compressionLevel - MIN_COMPRESSION_LEVEL].hashTableEntries;
    uint32_t moduloMask = hashTableEntries - 1;
    uint32_t maxChild = compressionParameters[compressionLevel - MIN_COMPRESSION_LEVEL].maxChild;
    hashTable = hashCreate(hashTableEntries, moduloMask);
    if(hashTable == NULL)
    {
        closeBitwiseBufferedFile(w);
        return -1;
    }
    if(writeBitBuffer(w, (uint32_t)compressionLevel, 3) == -1)
        goto exceptionHandler;
    while(!feof(inputFile) && !ferror(inputFile))
    {
        bufferedBytes = fread(readByte, 1, LOCAL_BYTE_BUFFER_LENGTH, inputFile);
        for(byteIndex = 0; byteIndex < bufferedBytes; byteIndex++)
        {
            child = hashLookup(hashTable, lookupIndex, readByte[byteIndex], moduloMask);
            if(child != ROOT_INDEX) lookupIndex = child; //ROOT_INDEX means NOT FOUND
            else
            {
                if(writeBitBuffer(w, lookupIndex, indexLength) == -1 || hashInsert(hashTable, lookupIndex, readByte[byteIndex], childIndex, moduloMask) == -1)
                    goto exceptionHandler;
                childIndex++;
                if((childIndex & indexLengthMask) == 0) //A power of 2 is reached
                {
                    //The length of the transmitted index is incremented
                    indexLength++;
                    //The next power of 2 mask is set
                    indexLengthMask = (indexLengthMask << 1) | 1;
                }
                //readByte value is also the right index to start with next time
                //because you have to start from the last character recognized
                lookupIndex = readByte[byteIndex] + 1; //ROOT_INDEX = 0 so ASCII characters are indexed in [1, 257]
                if (childIndex == maxChild) //hash table is full
                {
                    if(hashReset(hashTable, hashTableEntries, moduloMask) == NULL)
                        goto exceptionHandler; //hash table was not successfully created
                    childIndex = FIRST_CHILD; //starts from the beginning
                    indexLength = INITIAL_INDEX_LENGTH;
                    indexLengthMask = (1 << INITIAL_INDEX_LENGTH) - 1;
                }
            }
        }
    }
    if(ferror(inputFile))
    {
        errno = EBADFD;
        goto exceptionHandler;
    }
    if(writeBitBuffer(w, lookupIndex, indexLength) == -1 || writeBitBuffer(w, ROOT_INDEX, indexLength) == -1)
        goto exceptionHandler;
    hashDestroy(hashTable, hashTableEntries);
    return closeBitwiseBufferedFile(w);

    exceptionHandler:
        hashDestroy(hashTable, hashTableEntries);
        closeBitwiseBufferedFile(w);
        return -1;
}
