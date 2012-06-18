/**
 * @file Commons.c
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

#include "Commons.h"
#include <errno.h>

const uint32_t compressionLevelMatrix [5][3] =
{
        /* ******************************************************************* *
         * MAX_CHILD  |  HASH_TABLE_ENTRIES  |  HASH_TABLE_ENTRIES_MODULO_MASK *
         * ******************************************************************* */
        {  65535      ,  131072              ,  131071  },//2^16-1, 2^17, 2^17-1
        {  262143     ,  524288              ,  524287  },//2^18-1, 2^19, 2^19-1
        {  1048575    ,  2097152             ,  2097151 },//2^20-1, 2^21, 2^21-1
        {  4194303    ,  8388608             ,  8388607 },//2^22-1, 2^23, 2^23-1
        {  16777215   ,  33554432            ,  33554431} //2^24-1, 2^25, 2^25-1
};

uint32_t getCompressionParameter(int compressionLevel, int parameter)
{
    return (compressionLevel >= MIN_COMPRESSION_LEVEL && compressionLevel <= MAX_COMPRESSION_LEVEL
            && (parameter == MAX_CHILD || parameter == HASH_TABLE_ENTRIES || parameter == HASH_TABLE_ENTRIES_MODULO_MASK))?
            compressionLevelMatrix[compressionLevel - 1][parameter] : ((errno = EINVAL) && 0);
}
