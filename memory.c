#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "memory.h"
#include "util.h"

void init_memory(memory *mem) {
	mem->data = (char *)calloc(TOTAL_MEMORY, sizeof(char));
	memset(mem->data, '.', TOTAL_MEMORY);
	mem->total_storage = TOTAL_MEMORY;
	mem->free_storage = mem->total_storage;
	mem->most_recent_i = 0;
}

void free_memory(memory *mem) {
	free(mem->data);
}

/* 	copies process ID into memory from start to end.

	this function assumes the partition between start and
		end is ALL free memory
*/
void copy_memory(memory *mem, const char p_id, const int start, const int end) {
	memset(mem->data+start, p_id, end-start);
}

/*	removes process from memory  */
void remove_memory(memory *mem, const char p_id, const int p_mem) {
	int i, found;
	for(i = 0, found = 0; i < TOTAL_MEMORY; i++) {
		if(mem->data[i] == p_id) {
			found++;
		} else {
			if(found == p_mem) {
				copy_memory(mem, '.', i-p_mem, i);
				mem->free_storage += p_mem;
				break;
			}
			continue;
		}
	}
	printf("removed %d memory units; %d remaining\n", p_mem, mem->free_storage);
}

/* 	next fit memory algorithm. process is placed in the first
		free partition available.

	memory: the data structure containing memory
	p_id: the id of the process that needs to store memory
	p_mem: the amount of memory the process needs
*/
void add_memory_next_fit(memory *mem, const char p_id, const int p_mem) {
	int scanned = 0;
	int free_mem = 0;
	int max_free_mem = 0;

	int i;
	for(i = mem->most_recent_i; i < TOTAL_MEMORY || scanned < TOTAL_MEMORY; i++, scanned++) {
		if(i == TOTAL_MEMORY) {
			i = 0;
		}
		if(mem->data[i] == '.') { 
			free_mem++; 
			if(free_mem == p_mem) {
				copy_memory(mem, p_id, i-p_mem+1, i+1);
				mem->most_recent_i = i;
				mem->free_storage -= p_mem;
				break;
			}
		} else {
			max_free_mem = max(free_mem, max_free_mem);
			free_mem = 0;
		}
	}
	printf("stored %d memory units; %d remaining\n", p_mem, mem->free_storage);
	printf("found a max partition size of %d\n", max_free_mem);
}

/*	best fit memory algorithm. process is placed in the smallest
		free partition available
	
	memory: the data structure containing memory
	p_id: the id of the process that needs to store memory
	p_mem: the amount of memory the process needs
*/
void add_memory_best_fit(memory *mem, const char p_id, const int p_mem) {

}

/*	worst fit memory algorithm. process is placed in the largest
		free partition available that it can fit
	
	memory: the data structure containing memory
	p_id: the id of the process that needs to store memory
	p_mem: the amount of memory the process needs
*/
void add_memory_worst_fit(memory *mem, const char p_id, const int p_mem) {

}


void print_memory(memory *mem) {
	printf("================================\n");
	int i;
	for(i = 0; i < mem->total_storage; i++) {
		printf("%c", mem->data[i]);
		if((i + 1) % LINE_SIZE == 0)
			printf("\n");
	}
	printf("================================\n");
}