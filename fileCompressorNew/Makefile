headers = buildcodebook.h compress.h decompress.h huffman.h minheap.h hashmap.h 

sources = buildcodebook.c compress.c decompress.c huffman.c minheap.c hashmap.c 

all: $(headers) $(sources) filecompressor.c
	gcc filecompressor.c $(sources) -o fileCompressor