#include "memory_system.h"
#include <stdio.h>
#include "decoder.h"
#include "memory.h"
#include <string.h>
#include <stdlib.h>

void memory_store(int address, unsigned char value){
		unsigned char low = address &0X1F;
		unsigned char high = (address >> 5) &0X1F;
		unsigned int row = decoder(low);
		unsigned int col = decoder(high);
		mem_put(row, col, value);
}


void memory_store_word(int address, unsigned int value){
	unsigned int pair4 = value & 0xff;
	unsigned int pair3 = (value >> 8) & 0xff;
	unsigned int pair2 = (value >> 16) & 0xff;
	unsigned int pair1 = (value >> 24) & 0xff;

	for(int t = 0; t < 4; t++){
		unsigned char low = address &0X1F;
		unsigned char high = (address >> 5) &0X1F;
		unsigned int row = decoder(low);
		unsigned int col = decoder(high);

		if (t == 0){
			mem_put(row, col, pair1);
		}
		else if (t == 1){
			mem_put(row,col, pair2);
		}
		else if (t == 2){
			mem_put(row, col, pair3);
		}
		else{
			mem_put(row, col, pair4);
		}
		address++;
	}
}

unsigned char memory_fetch(int address){
		unsigned char low = address &0X1F;
		unsigned char high = (address >> 5) &0X1F;
		unsigned int row = decoder(low);
		unsigned int col = decoder(high);
		return mem_get(row,col);
}

unsigned int memory_fetch_word(int address){
	unsigned char pair1 = ' ';
	unsigned char pair2 = ' ';
	unsigned char pair3 = ' ';
	unsigned char pair4 = ' ';

	unsigned int pair1int = 0;
	unsigned int pair2int = 0;
	unsigned int pair3int = 0;
	unsigned int pair4int = 0;

	for(int t = 0; t < 4; t++){
		unsigned char low = address &0X1F;
		unsigned char high = (address >> 5) &0X1F;
		unsigned int row = decoder(low);
		unsigned int col = decoder(high);

		if (t == 0){
			pair1 = mem_get(row, col);
			pair1int = pair1;
		}
		else if (t == 1){
			pair2 = mem_get(row,col);
			pair2int = pair2;
		}
		else if (t == 2){
			pair3 = mem_get(row, col);
			pair3int = pair3;
		}
		else{
			pair4 = mem_get(row, col);
			pair4int = pair4;
		}
		address++;
	}

	return (pair1int << 24) + (pair2int << 16) + (pair3int << 8) + pair4int;
}

void memory_dump(int start_address, int num_bytes) {
	int start_boundary = start_address - (start_address % 8);
	int boundary_bytes = num_bytes + (start_address - start_boundary);
	printf("start_boundary: %d 0x%04x\n", start_boundary, start_boundary);
	for (int i = start_boundary; i < start_boundary+boundary_bytes; i+=4) {
		printf("0x%04x (0d%04d) 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", i, i, memory_fetch(i),   memory_fetch(i+1), memory_fetch(i+2), memory_fetch(i+3), memory_fetch(i+4), memory_fetch(i+5), memory_fetch(i+6), memory_fetch(i+7));
	}
}

void load_memory(char *filename){    
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL){
		perror("perror");
	}

	//read the first line(adddress)
	int address = 0;
	fscanf(fp, "%d", &address);
	printf("Starting address = %d \n", address);
	unsigned int buff = 0;
	while (fscanf(fp, "%x", &buff) != EOF){	
			printf("Passing in 0x%08x to address: %d\n", buff, address);
			printf("The unsigned int value is: %u\n", buff);
			memory_store_word(address, buff);
			address = address + 4;
	}
	fclose(fp);
}
