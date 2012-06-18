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
    uint32_t indexLengthMask = INDEX_LENGTH_MASK;
    uint32_t child;
    uint32_t hashTableEntries = getCompressionParameter(compressionLevel, HASH_TABLE_ENTRIES);
    uint32_t hashTableLength = hashTableEntries*sizeof(struct LZ78HashTableEntry);
    uint32_t moduloMask = getCompressionParameter(compressionLevel, HASH_TABLE_ENTRIES_MODULO_MASK);
    uint32_t maxChild = getCompressionParameter(compressionLevel, MAX_CHILD);
    if(!(maxChild && hashTableEntries && moduloMask) || inputFile == NULL || w == NULL)
    {
        errno = EINVAL;
        if(w != NULL) closeBitwiseBufferedFile(w);
        return -1;
    }
    hashTable = hashCreate(hashTableLength, moduloMask);
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
                    if(hashReset(hashTable, hashTableLength, moduloMask) == NULL)
                        goto exceptionHandler; //hash table was not successfully created
                    childIndex = FIRST_CHILD; //starts from the beginning
                    indexLength = INITIAL_INDEX_LENGTH;
                    indexLengthMask = INDEX_LENGTH_MASK;
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
    hashDestroy(hashTable, hashTableLength);
    return closeBitwiseBufferedFile(w);

    exceptionHandler:
        hashDestroy(hashTable, hashTableLength);
        closeBitwiseBufferedFile(w);
        return -1;
}
