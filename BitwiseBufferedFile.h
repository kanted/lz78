/**
 * @file BitwiseBufferedFile.h
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

#include <sys/types.h>
#include <stdint.h>

#ifndef BITWISE_BUFFERED_FILE
#define BITWISE_BUFFERED_FILE

struct BitwiseBufferedFile;

/**
 * @brief BitwiseBufferedFile open function
 *
 * This function creates a new BitwiseBufferedFile structure. The related file
 * could be specified, in descending order of priority, by path, by file
 * descriptor or through the related FILE structure. When pathToFile is set,
 * the mode parameter defines the opening mode.
 * If O_WRONLY is passed and the specified file does not exist,
 * it is also created.
 * @param path The pathToFile of the file to open, or NULL to use other opening techniques.
 * @param mode O_RDONLY (read only) or O_WRONLY (write only).
 * @param fileDescriptorToSet The referred file descriptor, the file must be opened according to mode or be -1 to use other opening techniques.
 * @param fileToSet The referred file FILE struct pointer, the file must be opened according to mode or be NULL to use other opening techniques.
 **/
struct BitwiseBufferedFile* openBitwiseBufferedFile(const char* pathToFile, int mode, int fileDescriptorToSet, FILE* fileToSet);

/**
 * @brief BitwiseBufferedFile close function
 *
 * This function closes the passed BitwiseBufferedFile. If it had been opened in
 * write mode, the remaining buffered data will be flushed into the file.
 * @param bitFile The BitwiseBufferedFile to close.
 **/
int closeBitwiseBufferedFile(struct BitwiseBufferedFile* bitFile);

/**
 * @brief Read some bits.
 *
 * Bitwise read. Up to length bits are read from bitFile and stored into data.
 * If bitFile had not been opened in read mode, an error will occur.
 * @param bitFile The BitwiseBufferedFile to read.
 * @param data Small buffer to hold the read bits.
 * @param length The number of bits to read.
 **/
ssize_t readBitBuffer(struct BitwiseBufferedFile* bitFile, uint32_t* data, size_t length);

/**
 * @brief Write some bits.
 *
 * Bitwise write. Up to length bits are written to bitFile.
 * If bitFile had not been opened in write mode, an error will occur.
 * @param bitFile The BitwiseBufferedFile to write.
 * @param data Small buffer to hold the bits to be written.
 * @param length The number of bits to write.
 **/
ssize_t writeBitBuffer(struct BitwiseBufferedFile* bitFile, uint32_t data, int length);

#endif
