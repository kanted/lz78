/**
 * @file main.c
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
#include "LZ78Decompressor.h"
#include "Commons.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

const char* license =
"lz78 rev 132\n\
Copyright (C) 2012 Cosimo Sacco and Davide Silvestri.\n\
This is free software.\n\
You may redistribute copies of it under the terms of\n\
the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n\
There is NO WARRANTY, to the extent permitted by law.\n";

const char* help =
"Usage: lz78 [OPTION]...\n\
Compress or uncompress FILE.\n\n\
Mandatory arguments to long options are mandatory for short options too.\n\n\
  -c, --compress    compress the specified file\n\
  -d, --decompress  decompress the specified file\n\
  -l, --level       set compression level [1-5]\n\
      --fast        compress faster [1]\n\
      --best        compress better [5]\n\
  -L, --license     display software license\n\
  -h, --help        display this message\n\
  -V, --version     display version number\n\n\
Report bugs to <cosimosacco@gmail.com> <davidesil.web@gmail.com>\n";

static int fastFlag = 0;
static int bestFlag = 0;

int main(int argc, char** argv)
{
    int optionToProcess = 0;
    int compressFlag = -1;
    int compressionLevel = 0;
    char* inputFile = NULL;
    char* outputFile = NULL;
    FILE* input;
    FILE* output;
    struct stat inputStat;
    struct stat outputStat;
    int error = 0;
    int optionIndex = 0;
    static struct option options[] =
    {
        {"best"      , no_argument      , &bestFlag,  1 },
        {"fast"      , no_argument      , &fastFlag,  1 },
        {"help"      , no_argument      , 0        , 'h'},
        {"license"   , no_argument      , 0        , 'L'},
        {"level"     , required_argument, 0        , 'l'},
        {"compress"  , required_argument, 0        , 'c'},
        {"decompress", required_argument, 0        , 'd'},
        {"output"    , required_argument, 0        , 'o'},
        {0           , 0                , 0        ,  0 }
    };
    while(optionToProcess != -1)
    {
        optionToProcess = getopt_long(argc, argv, "hLl:c:d:o:", options, &optionIndex);
        switch (optionToProcess)
        {
            case 0:
            case 1:
                if(optarg)
                {
                    fprintf(stderr, "No argument expected for option %s\n", options[optionIndex].name);
                    return 1;
                }
                compressionLevel = (optionToProcess == 1) ? 1 : 5;
                break;
            case 'h':
                printf(help);
                return 0;
            case 'L':
                printf(license);
                return 0;
            case 'l':
                compressionLevel = atoi(optarg);
                if(compressionLevel < MIN_COMPRESSION_LEVEL || compressionLevel > MAX_COMPRESSION_LEVEL)
                {
                    fprintf(stderr, "Invalid compression level\n");
                    return 1;
                }
                break;
            case 'c':
                inputFile = optarg;
                compressFlag = 1;
                break;
            case 'd':
                inputFile = optarg;
                compressFlag = 0;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case -1:
                break;
            default:
                abort();
                break;
        }
    }
    if (optind < argc)
    {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }
    if(compressFlag == -1)
        return 0;
    if(compressionLevel == 0)
        compressionLevel = DEFAULT_COMPRESSION_LEVEL;
    if(outputFile == NULL)
        outputFile = "out.lz78";
    input = fopen(inputFile,"r");
    output = (strcmp(inputFile, outputFile) == 0)? NULL : fopen(outputFile,"w+");
    if(input == NULL || output == NULL)
    {
        fprintf(stderr, "Invalid file passed\n");
        return 1;
    }
    error = (compressFlag)? compress(input, output, compressionLevel) : decompress(input, output);
    fclose(input);
    fclose(output);
    if(compressFlag)
    {
        if(!(stat(inputFile, &inputStat) || stat(outputFile, &outputStat)) && (outputStat.st_size > inputStat.st_size))
        {
            fprintf(stdout, "File not compressed\n");
            remove(outputFile);
            return 1;
        }
    }
    if(error)
    {
        fprintf(stderr, "Compression error\n");
        remove(outputFile);
    }
    return error;
}
