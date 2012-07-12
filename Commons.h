/**
 * @file Commons.h
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

#include <stdint.h>

#ifndef LZ78_COMMONS
#define LZ78_COMMONS
 /**
 * ROOT_INDEX: index of the root of the compression table.
 **/
#define ROOT_INDEX 0
/**
 * FIRST_CHILD: number of valid initial entries, one entry for each possible
 * byte (256) plus one entry for the root of the compression table.
 **/
#define FIRST_CHILD 257
/**
 * INITIAL_INDEX_LENGTH: at the beginning there are 257 possible indexes
 * (256 which refer entries with a single byte, one wich refers the root).
 * So, at the beginning, ceiling(log_2(INITIAL_INDEX_LENGTH)) bits are
 * sufficient to index each index. This value is valid until there are less
 * than 512 entries, so this constant is an initializer for a variable
 * which holds the current index length.
 * At the beginning, FIRST_CHILD entries are present,
 * which can be addressed using an index with INITIAL_INDEX_LENGTH bits.
 * When the number of entries increases, at a certain point, the index
 * length must be updated. This happens when the number of entries reaches
 * the next "least bigger" power of two: in that case, the index length must
 * be increased by 1. When the rest of the division between the current
 * number of entries and the next "least bigger" power of two is zero,
 * then the current number of entries reached the next power of two and the
 * index length must be increased. Then, the next power of two is calculated
 * and used until the number of entries reach it, and so on.
 * This test involves division. Since both the "least bigger" power of two
 * and the current number of entries are variables, the compiler, being
 * unaware of their runtime values, cannot optimize the division and the
 * modulo operation. So, the value ((1 << INITIAL_INDEX_LENGTH) - 1) is used to
 * initialize a variable which holds a mask related to the current
 * "least bigger" power of two, and can be used to calculate
 * the modulo. In order to do so, a simple bitwise and between those
 * variables is sufficient. When the current power of two is reached, the
 * mask has to be updated. If the current power of two is 2^k, the mask
 * is (2^k)-1, and it is updated as follows: mask = (mask << 1) | 1.
 **/
#define INITIAL_INDEX_LENGTH 9 //ceiling(log_2(257))
#define MIN_COMPRESSION_LEVEL 1
#define DEFAULT_COMPRESSION_LEVEL 3
#define MAX_COMPRESSION_LEVEL 5
#define HEADER_LENGTH 3

struct CompressionParameterTuple
{
    uint32_t maxChild;
    uint32_t hashTableEntries;
};

extern const struct CompressionParameterTuple compressionParameters[MAX_COMPRESSION_LEVEL - MIN_COMPRESSION_LEVEL + 1];

#endif
