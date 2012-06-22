/**
 * @file LZ78HashTable.h
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

#ifndef LZ78_HASH_TABLE
#define LZ78_HASH_TABLE

struct LZ78HashTableEntry;

struct LZ78HashTableEntry* hashInitialize(struct LZ78HashTableEntry*, uint32_t, uint32_t);

struct LZ78HashTableEntry* hashCreate(uint32_t, uint32_t);

uint32_t hashLookup(struct LZ78HashTableEntry*, uint32_t, uint32_t, uint32_t);

int hashInsert(struct LZ78HashTableEntry*, uint32_t, uint32_t, uint32_t, uint32_t);

struct LZ78HashTableEntry* hashReset(struct LZ78HashTableEntry*, uint32_t, uint32_t);

void hashDestroy(struct LZ78HashTableEntry*, uint32_t);

#endif
