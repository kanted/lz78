/**
 * @file LZ78DecompressorTable.c
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

#include "LZ78DecompressorTable.h"
#include "Commons.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void tableDestroy(struct LZ78DecompressorTableEntry* table, uint32_t maxChild)
{
    if(table == NULL)
        return;
    int i = maxChild - 1;
    for(; i--;)
        free(table[i].word);
    memset(table, 0, sizeof(struct LZ78DecompressorTableEntry)*maxChild);
    free(table);
}

struct LZ78DecompressorTableEntry* tableCreate(uint32_t maxChild)
{
    struct LZ78DecompressorTableEntry* table = calloc(maxChild, sizeof(struct LZ78DecompressorTableEntry));
    /**
     * The previous allocation automatically sets the default values:
     * each fatherIndex becomes 0 (== ROOT_INDEX)
     * each word becomes 0 (== NULL)
     **/
    int i = 1;
    struct LZ78DecompressorTableEntry* current;
    if(table != NULL)
    {
        for(; i < FIRST_CHILD; i++)
        {
            current = &(table[i]);
            current->length = 1;
            current->word = malloc(1);
            if(current->word == NULL)
            {
                tableDestroy(table, maxChild);
                table = NULL;
                break;
            }
            current->word[0] = (uint8_t)i - 1;
        }
    }
    return table;
}
