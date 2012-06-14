/**
 * @file LZ78DecompressorTable.h
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

#include "../../Configuration/LZ78CompressorConfiguration.h"
#include "../../../lib/BitwiseBufferedFile/bufferConfiguration.h"
#include <stdint.h>

#ifndef LZ78_DECOMPRESSOR_TABLE
#define LZ78_DECOMPRESSOR_TABLE

struct LZ78DecompressorTableEntry
{
    uint8_t* word;
    INDEX_TYPE fatherIndex;
    CELL_TYPE length;
    CELL_TYPE bufferLength;
};

inline struct LZ78DecompressorTableEntry* tableCreate(uint32_t maxChild);

inline void tableDestroy(struct LZ78DecompressorTableEntry* table, uint32_t maxChild);

#endif
