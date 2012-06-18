/**
 * @file LZ78Compressor.h
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

#include <stdio.h>

#ifndef LZ78_COMPRESSOR
#define LZ78_COMPRESSOR

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

    int compress(FILE* inputFile, FILE* outputFile, int compressionLevel);

#endif
