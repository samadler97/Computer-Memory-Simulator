#include "bit_functions.h"
#include "cpu.h"
//#include "memory_system.c"
#include <stdio.h>
#include <stdlib.h>


int registers[16];
int cpsr=0;
int z = 0;
int lt = 0;
int gt = 0;

char *op_codes[15] = {"nothing", "LDR", "LDI", "LDX", "STR", "ADD", "SUB", "MUL", "DIV", "CMP", "B", "BEQ", "BNE", "BLT", "BGT"}; 

void set_reg(int reg, int value){
	registers[reg] = value;
}


int get_reg(int reg){
	return registers[reg];
}


int get_cpsr(){
	return cpsr;
}

void show_regs(){
	printf("R0: 0x%08x, R1: 0x%08x, R2: 0x%08x, R3: 0x%08x\n", registers[0], registers[1], registers[2], registers[3]);
	printf("R4: 0x%08x, R5: 0x%08x, R6: 0x%08x, R7: 0x%08x\n", registers[4], registers[5], registers[6], registers[7]);
	printf("R8: 0x%08x, R9: 0x%08x, R10: 0x%08x, R11: 0x%08x\n", registers[8], registers[9], registers[10], registers[11]);
	printf("R12: 0x%08x, R13: 0x%08x, R14: 0x%08x, R15: 0x%08x\n", registers[12], registers[13], registers[14], registers[15]);
}


void step(){
	
	int pc = registers[PC];
	unsigned int inst = memory_fetch_word(registers[PC]);
	int opcode = inst >> 24;
	if (opcode > 15){
		printf("Opcode out of bounds.\n");
		exit(1);
	}
	printf("PC: 0x%08x, inst: 0x%08x, opcode: %s\n", registers[PC], inst, op_codes[opcode]);
	int reg, reg1, reg2, dest_reg, address, immediate, offset;
	switch(opcode){
		case LDR:
			reg = inst >> 16 & 0xff;
			address = inst & 0xffff;
			int intAddress = address;
			int tylerReg = memory_fetch_word(intAddress);
			printf("We are trying to store %d in register number %d from address: %d\n", tylerReg, reg, intAddress);
			if (address > 1023 || reg > 15){
				printf("Address/Register out of bounds.\n");
				exit(1);
			}
			registers[reg] = memory_fetch_word(address);
			//printf("Register number: %d, register value: 0x%08x, address: 0x%04x\n", reg, registers[reg], address);
			pc += 4;
			set_reg(PC, pc); 
			break;
		case LDI:
			reg = inst >> 16 & 0xff;
			immediate = inst & 0xffff;
			set_reg(reg, immediate);
			printf("Succesfully loaded the immediate value of %d into register: %d\n", immediate, reg);
			pc += 4;
			set_reg(PC, pc); 
			break;
		case LDX:
			dest_reg = inst >> 16 & 0xff;
			offset = inst >> 8 & 0xff;
			reg = inst & 0xff;
			int newRegTyler = registers[reg];
			int newAddress = newRegTyler + offset;
			int valueOf = memory_fetch_word(newAddress);
			set_reg(dest_reg, valueOf);
			printf("Succesfully loaded the desired value 0x%08x into register %d\n", valueOf, dest_reg);
			pc += 4;
			set_reg(PC, pc);
			break;
		case ADD:
			dest_reg = inst >> 16 & 0xff;
			printf("The destination register is: %d\n", dest_reg);
			reg1 = inst >> 8 & 0xff;
			printf("The first register is: %d and the value is %d\n", reg1, registers[reg1]);
			reg2 = inst & 0xff;
			printf("The second register is: %d and the value is %d\n", reg2, registers[reg2]);
			if (dest_reg > 15 || reg1 > 15 || reg2 > 15){
				printf("Address/Register out of bounds.\n");
				exit(1);
			}
			int addition = registers[reg1] + registers[reg2];
			set_reg(dest_reg, addition);
			printf("Succesfully put the value of %d into register %d\n", registers[dest_reg], dest_reg);
			pc += 4;
			set_reg(PC, pc); 
			break;
		case SUB:
			dest_reg = inst >> 16 & 0xff;
			printf("The destination register is: %d\n", dest_reg);
			reg1 = inst >> 8 & 0xff;
			printf("The first register is: %d and the value is %d\n", reg1, registers[reg1]);
			reg2 = inst & 0xff;
			printf("The second register is: %d and the value is %d\n", reg2, registers[reg2]);
			if (dest_reg > 15 || reg1 > 15 || reg2 > 15){
				printf("Address/Register out of bounds.\n");
				exit(1);
			}
			int subtraction = registers[reg1] - registers[reg2];
			set_reg(dest_reg, subtraction);
			printf("Succesfully put the value of %d into register %d\n", registers[dest_reg], dest_reg);
			pc += 4;
			set_reg(PC, pc); 
			break;
		case CMP:
			reg1 = inst >> 8 & 0xff;
            reg2 = inst & 0xff;
            if (reg1 > 15 || reg2 > 15) {
                printf("Register out of bounds.\n");
                exit(1);
            }
            
            z = registers[reg1] == registers[reg2];
            lt = registers[reg1] <  registers[reg2];
            gt = registers[reg1] >  registers[reg2];
            
            printf("z: %d, lt: %d, gt: %d, reg1: %d, reg2: %d\n", z, lt, gt, registers[reg1], registers[reg2]);
            
            if (z) {
                bit_set(&cpsr, Z);
                bit_clear(&cpsr, LT);
                bit_clear(&cpsr, GT);
            }
            if (lt) {
                bit_set(&cpsr, LT);
                bit_clear(&cpsr, Z);
                bit_clear(&cpsr, GT);
            }
            if (gt) {
                bit_set(&cpsr, GT);
                bit_clear(&cpsr, Z);
                bit_clear(&cpsr, LT);
            }
            pc += 4;
            set_reg(PC, pc);
            break;
		case BGT:
			printf("The current value of CPSR is %d\n", cpsr);
			address = inst & 0xffffff;
			if (address > 1023){
				printf("Address/Register out of bounds.\n");
				exit(1);
			}
			if (gt == 1){
				printf("Succesfully branching to address %d\n", address);
				set_reg(PC, address);
				break;
			}
			else{
				printf("Less than clause not met, moving to next program counter.\n");
				pc = pc + 4;
				set_reg(PC, pc);
				break;
			}
            break;
			
		case BLT:
			address = inst & 0xffffff;
			if (address > 1023){
				printf("Address/Register out of bounds.\n");
				exit(1);
			}
			if (lt == 1){
				printf("Succesfully branching to address %d\n", address);
				set_reg(PC, address);
				break;
			}
			else{
				printf("Less than clause not met, moving to next program counter.\n");
				pc = pc + 4;
				set_reg(PC, pc);
				break;
			}
            break;
            
		case BNE:
			address = inst & 0xffffff;
			if (address > 1023){
				printf("Address/Register out of bounds.\n");
				exit(1);
			}
			if (z != 1){
				printf("Succesfully branching to address %d\n", address);
				set_reg(PC, address);
				break;
			}
			else{
				printf("Less than clause not met, moving to next program counter.\n");
				pc = pc + 4;
				set_reg(PC, pc);
				break;
			}
            break;
            
		case BEQ:
			address = inst & 0xffffff;
			if (address > 1023){
				printf("Address/Register out of bounds.\n");
				exit(1);
			}
			if (z == 1){
				printf("Succesfully branching to address %d\n", address);
				set_reg(PC, address);
				break;
			}
			else{
				printf("Less than clause not met, moving to next program counter.\n");
				pc = pc + 4;
				set_reg(PC, pc);
				break;
			}
            break;
			
		case B:
				address = inst & 0xffffff;
				if (address > 1023){
				printf("Address/Register out of bounds.\n");
				exit(1);
				}
				pc = address + 4;
				set_reg(PC, pc);
				break;
		}
}


void step_n(int n){
	set_reg(PC, 348);
	for(int j = 0; j < n; j++){
		step();
	}
	
}
