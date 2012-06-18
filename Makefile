CC = gcc
CFLAGS = -O2 -Wall -Werror 
OBJS = BitwiseBufferedFile.o Commons.o LZ78HashTable.o LZ78DecompressorTable.o LZ78Compressor.o LZ78Decompressor.o main.o
TARGET = lz78

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	-rm `find -name "*.o"` $(TARGET)
