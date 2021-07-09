#include "memory.h"
#include "bit_functions.h"
#include <stdio.h>

unsigned char memory_chip[32][32];

unsigned char mem_get(unsigned int row_selected, unsigned int col_selected){
	int row = bit_find(row_selected);
	int col = bit_find(col_selected);
	return memory_chip[row][col];
}

void mem_put(unsigned int row_selected, unsigned int col_selected, unsigned char b){
	int row = bit_find(row_selected);
	int col = bit_find(col_selected);
	memory_chip[row][col] = b;
}
