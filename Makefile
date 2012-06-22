CC = gcc
CFLAGS = -O2 -Wall -Werror
OBJS = BitwiseBufferedFile.o Commons.o LZ78HashTable.o LZ78DecompressorTable.o LZ78Compressor.o LZ78Decompressor.o lz78.o
TARGET = lz78

$(TARGET): $(OBJS)

BitwiseBufferedFile.o: BitwiseBufferedFile.h mathUtils.h

Commons.o: Commons.h

LZ78HashTable.o: LZ78HashTable.h Commons.h

LZ78DecompressorTable.o: LZ78DecompressorTable.h Commons.h

LZ78Compressor.o: LZ78Compressor.h LZ78HashTable.h Commons.h BitwiseBufferedFile.h

LZ78Decompressor.o: LZ78Decompressor.h Commons.h LZ78DecompressorTable.h BitwiseBufferedFile.h

lz78.o: LZ78Compressor.h LZ78Decompressor.h Commons.h

clean:
	-rm `find -name "*.o"` $(TARGET)
