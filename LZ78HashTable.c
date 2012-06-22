/**
 * @file LZ78HashTable.c
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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "LZ78HashTable.h"
#include "Commons.h"

struct LZ78HashTableEntry
{
    //key:
    uint32_t fatherIndex;
    uint32_t childValue; //Only one byte is used
    //value:
    uint32_t childIndex;
};

const uint32_t sBox[] =
{
    0xF53E1837, 0x5F14C86B, 0x9EE3964C, 0xFA796D53, 0x32223FC3, 0x4D82BC98,
    0xA0C7FA62, 0x63E2C982, 0x24994A5B, 0x1ECE7BEE, 0x292B38EF, 0xD5CD4E56,
    0x514F4303, 0x7BE12B83, 0x7192F195, 0x82DC7300, 0x084380B4, 0x480B55D3,
    0x5F430471, 0x13F75991, 0x3F9CF22C, 0x2FE0907A, 0xFD8E1E69, 0x7B1D5DE8,
    0xD575A85C, 0xAD01C50A, 0x7EE00737, 0x3CE981E8, 0x0E447EFA, 0x23089DD6,
    0xB59F149F, 0x13600EC7, 0xE802C8E6, 0x670921E4, 0x7207EFF0, 0xE74761B0,
    0x69035234, 0xBFA40F19, 0xF63651A0, 0x29E64C26, 0x1F98CCA7, 0xD957007E,
    0xE71DDC75, 0x3E729595, 0x7580B7CC, 0xD7FAF60B, 0x92484323, 0xA44113EB,
    0xE4CBDE08, 0x346827C9, 0x3CF32AFA, 0x0B29BCF1, 0x6E29F7DF, 0xB01E71CB,
    0x3BFBC0D1, 0x62EDC5B8, 0xB7DE789A, 0xA4748EC9, 0xE17A4C4F, 0x67E5BD03,
    0xF3B33D1A, 0x97D8D3E9, 0x09121BC0, 0x347B2D2C, 0x79A1913C, 0x504172DE,
    0x7F1F8483, 0x13AC3CF6, 0x7A2094DB, 0xC778FA12, 0xADF7469F, 0x21786B7B,
    0x71A445D0, 0xA8896C1B, 0x656F62FB, 0x83A059B3, 0x972DFE6E, 0x4122000C,
    0x97D9DA19, 0x17D5947B, 0xB1AFFD0C, 0x6EF83B97, 0xAF7F780B, 0x4613138A,
    0x7C3E73A6, 0xCF15E03D, 0x41576322, 0x672DF292, 0xB658588D, 0x33EBEFA9,
    0x938CBF06, 0x06B67381, 0x07F192C6, 0x2BDA5855, 0x348EE0E8, 0x19DBB6E3,
    0x3222184B, 0xB69D5DBA, 0x7E760B88, 0xAF4D8154, 0x007A51AD, 0x35112500,
    0xC9CD2D7D, 0x4F4FB761, 0x694772E3, 0x694C8351, 0x4A7E3AF5, 0x67D65CE1,
    0x9287DE92, 0x2518DB3C, 0x8CB4EC06, 0xD154D38F, 0xE19A26BB, 0x295EE439,
    0xC50A1104, 0x2153C6A7, 0x82366656, 0x0713BC2F, 0x6462215A, 0x21D9BFCE,
    0xBA8EACE6, 0xAE2DF4C1, 0x2A8D5E80, 0x3F7E52D1, 0x29359399, 0xFEA1D19C,
    0x18879313, 0x455AFA81, 0xFADFE838, 0x62609838, 0xD1028839, 0x0736E92F,
    0x3BCA22A3, 0x1485B08A, 0x2DA7900B, 0x852C156D, 0xE8F24803, 0x00078472,
    0x13F0D332, 0x2ACFD0CF, 0x5F747F5C, 0x87BB1E2F, 0xA7EFCB63, 0x23F432F0,
    0xE6CE7C5C, 0x1F954EF6, 0xB609C91B, 0x3B4571BF, 0xEED17DC0, 0xE556CDA0,
    0xA7846A8D, 0xFF105F94, 0x52B7CCDE, 0x0E33E801, 0x664455EA, 0xF2C70414,
    0x73E7B486, 0x8F830661, 0x8B59E826, 0xBB8AEDCA, 0xF3D70AB9, 0xD739F2B9,
    0x4A04C34A, 0x88D0F089, 0xE02191A2, 0xD89D9C78, 0x192C2749, 0xFC43A78F,
    0x0AAC88CB, 0x9438D42D, 0x9E280F7A, 0x36063802, 0x38E8D018, 0x1C42A9CB,
    0x92AAFF6C, 0xA24820C5, 0x007F077F, 0xCE5BC543, 0x69668D58, 0x10D6FF74,
    0xBE00F621, 0x21300BBE, 0x2E9E8F46, 0x5ACEA629, 0xFA1F86C7, 0x52F206B8,
    0x3EDF1A75, 0x6DA8D843, 0xCF719928, 0x73E3891F, 0xB4B95DD6, 0xB2A42D27,
    0xEDA20BBF, 0x1A58DBDF, 0xA449AD03, 0x6DDEF22B, 0x900531E6, 0x3D3BFF35,
    0x5B24ABA2, 0x472B3E4C, 0x387F2D75, 0x4D8DBA36, 0x71CB5641, 0xE3473F3F,
    0xF6CD4B7F, 0xBF7D1428, 0x344B64D0, 0xC5CDFCB6, 0xFE2E0182, 0x2C37A673,
    0xDE4EB7A3, 0x63FDC933, 0x01DC4063, 0x611F3571, 0xD167BFAF, 0x4496596F,
    0x3DEE0689, 0xD8704910, 0x7052A114, 0x068C9EC5, 0x75D0E766, 0x4D54CC20,
    0xB44ECDE2, 0x4ABC653E, 0x2C550A21, 0x1A52C0DB, 0xCFED03D0, 0x119BAFE2,
    0x876A6133, 0xBC232088, 0x435BA1B2, 0xAE99BBFA, 0xBB4F08E4, 0xA62B5F49,
    0x1DA4B695, 0x336B84DE, 0xDC813D31, 0x00C134FB, 0x397A98E6, 0x151F0E64,
    0xD9EB3E69, 0xD3C7DF60, 0xD2F2C336, 0x2DDD067B, 0xBD122835, 0xB0B3BD3A,
    0xB0D54E46, 0x8641F1E4, 0xA0B38F96, 0x51D39199, 0x37A6AD75, 0xDF84EE41,
    0x3C034CBA,             0xACDA62FC, 0x11923B8B,             0x45EF170A
};

//S-BOX hash function
uint32_t hashFunction(uint32_t key1, uint32_t key2, uint32_t moduloMask)
{
    uint32_t key = (key1 << 8) | key2;
    uint8_t* keyArray = ((uint8_t*)&key);
    return (sBox[keyArray[0]] ^ sBox[keyArray[1]] ^ sBox[keyArray[2]] ^ sBox[keyArray[3]]) & moduloMask;
}

/*
const uint8_t permutationTable[256] =
{
      0,  98, 207, 203, 230,  76, 194,  50, 136, 126, 142, 202,  24,  58, 253,
    174,  69, 161,   9,   8, 109,  62, 191, 244, 215,  22, 119,  52, 129,  40,
    188,  35, 173,  13,  49, 242,  81,  28,  38, 159,  57,  84,   2, 214, 137,
     42, 252,  31,  66, 186, 135, 223, 169, 247,  96,  53,  82,  21, 110, 112,
    167, 164, 100,  56, 200, 147,  14,  85,  23, 178,  51,  70, 199, 101, 196,
    182,   7, 239,  72,  17,  79, 151,  27, 205, 248, 140,  91,  39,  32,  25,
     63,  67, 128, 228,  12,  48, 150,  46,  88, 121, 139, 130, 184,  11, 124,
     90, 233, 180, 120, 179, 132, 218,  41, 113, 177, 141, 138, 187, 172, 206,
    240, 146, 158,  78, 225, 176, 108, 246, 171, 148,  44,   4, 234, 166, 115,
    156, 189, 106, 104, 221, 149,  43,  26, 231,  55, 251, 217, 211, 229, 190,
    111, 133, 216, 118,  16, 192, 162, 175, 105,   3, 122, 123,  95,  64, 237,
    195,  75,  68, 170,  37,  65, 145, 185,  83, 116,  15, 157, 127,  45, 204,
    245, 243,   1, 153,  86, 224, 209, 155,  29, 193, 198, 102,  97, 226, 201,
    227,  10, 117, 222, 213, 154, 107, 131, 143,  74, 103,  99,  93, 255, 236,
    232, 152, 134,  47,  60,  94, 168,  18,  59, 210,   5,  30,  19, 249, 241,
    197, 160, 183, 219, 208,  89, 238, 165,   6, 254,  33,  92, 163,  80,  71,
     36, 144, 212, 114, 250,  34, 235,  73,  77, 181,  54, 220,  61,  20, 125
};


inline uint32_t PEARSONhashFunction(uint32_t key1, uint32_t key2) //PEARSON hash function
{
    int i = 0;
    uint8_t index = 0;
    uint32_t key = (((uint32_t)key1) << (sizeof(uint32_t)*8)) | ((uint32_t)key2);
    uint8_t* keyArray = ((uint8_t*)&key);
    for(; i < sizeof(uint32_t); i++)
    {
        index = permutationTable[index] ^ keyArray[i];
        keyArray[i] = permutationTable[index];
    }
    return key%(HASH_TABLE_ENTRIES);
}

uint32_t SAXhashFunction(uint32_t key1, uint32_t key2) //SAX hash function
{
    uint32_t index = 0;
    uint32_t key = (((uint32_t)key1) << (sizeof(uint32_t)*8)) | ((uint32_t)key2);
    uint8_t* keyArray = ((uint8_t*)&key);
    int i = 0;
    for (; i < sizeof(uint32_t) ; i++)
        index ^= (index << 5) + (index >> 2) + ((uint32_t)(keyArray[i]));
    index %= HASH_TABLE_ENTRIES; //TODO lento
    return index;
}

uint32_t BARNSTEINhashFunction(uint32_t key1, uint32_t key2) //Bernstein hash function
{
    uint32_t index = 0;
    uint32_t key = (((uint32_t)key1) << (sizeof(uint32_t)*8)) | ((uint32_t)key2);
    uint8_t* keyArray = ((uint8_t*)&key);
    int i = 0;
    for (; i < sizeof(uint32_t) ; i++)
        index = 33 * index + ((uint32_t)(keyArray[i]));
    index %= HASH_TABLE_ENTRIES; //TODO lento
    return index;
}

uint32_t ELFhashFunction(uint32_t key1, uint32_t key2) //ELF hash function
{
    uint32_t index = 0;
    uint32_t g = 0;
    uint32_t key = (((uint32_t)key1) << (sizeof(uint32_t)*8)) | ((uint32_t)key2);
    uint8_t* keyArray = ((uint8_t*)&key);
    int i = 0;
    for (; i < sizeof(uint32_t) ; i++){
        index = ( index << 4 ) + ((uint32_t)(keyArray[i]));
    g = index & 0xf0000000L;
    if ( g != 0 )
        index ^= g >> 24;
    index &= ~g;
    }
    index %= HASH_TABLE_ENTRIES; //TODO lento
    return index;
}
*/

int hashInsert(struct LZ78HashTableEntry* table, uint32_t fatherIndex, uint32_t childValue, uint32_t childIndex, uint32_t moduloMask)
{
    if(table == NULL)
        return -1;
    struct LZ78HashTableEntry* current;
    uint32_t index = hashFunction(fatherIndex, childValue, moduloMask);
    //Until there's no collision (childIndex != ROOT_INDEX), try the next entry
    while(table[index].childIndex)
        index = (index+1) & moduloMask;
    //Since the meaningful entries are, at most, MAX_CHILD, while the total
    //entries are HASH_TABLE_ENTRIES > MAX_CHILD, an empty entry is granted
    //to be found
    current = &(table[index]);
    current->childIndex = childIndex;
    current->fatherIndex = fatherIndex;
    current->childValue = childValue;
    return 0;
}

uint32_t hashLookup(struct LZ78HashTableEntry* table, uint32_t fatherIndex, uint32_t childValue, uint32_t moduloMask)
{
    if(table == NULL)
        return -1;
    uint32_t index = hashFunction(fatherIndex, childValue, moduloMask);
    //slow but it's done only in case of collision, the loop condition is: !empty(entry) && !equal(key, entry.key)
    while((table[index].childIndex && (table[index].childValue != childValue || table[index].fatherIndex != fatherIndex)))
        index = (index+1) & moduloMask;
    return table[index].childIndex;
}

struct LZ78HashTableEntry* hashInitialize(struct LZ78HashTableEntry* table, uint32_t hashTableEntries, uint32_t moduloMask)
{
    int i = 0;
    uint32_t currentValue = 0;
    if(table != NULL)
    {
        memset(table, 0, hashTableEntries*sizeof(struct LZ78HashTableEntry));
        for(; i < 256; i++)
        {
            currentValue = i;  //ascii value - 1 equals to index value
            if(hashInsert(table, ROOT_INDEX, currentValue, currentValue + 1, moduloMask) == -1)
                goto exceptionHandler;
        }
    }
    return table;

    exceptionHandler:
        hashDestroy(table,hashTableEntries);
        return NULL;
}

struct LZ78HashTableEntry* hashCreate(uint32_t hashTableEntries, uint32_t moduloMask)
{
    return hashInitialize((struct LZ78HashTableEntry*)malloc(hashTableEntries*sizeof(struct LZ78HashTableEntry)), hashTableEntries, moduloMask);
}

struct LZ78HashTableEntry* hashReset(struct LZ78HashTableEntry* table, uint32_t hashTableEntries, uint32_t moduloMask)
{
    return hashInitialize(table, hashTableEntries, moduloMask);
}

void hashDestroy(struct LZ78HashTableEntry* table, uint32_t hashTableEntries)
{
    memset(table, 0, hashTableEntries*sizeof(struct LZ78HashTableEntry));
    free(table);
}
