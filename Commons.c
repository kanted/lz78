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

const struct CompressionParameterTuple compressionParameters[MAX_COMPRESSION_LEVEL - MIN_COMPRESSION_LEVEL + 1] =
{
    {65535, 131072}, {262143, 524288}, {1048575, 2097152}, {4194303, 8388608}, {16777215, 33554432}
};
