#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "memory.h"

void init_memory(memory *mem) {
	mem->total_storage = TOTAL_MEMORY;
	mem->data = (char *)calloc(mem->total_storage, sizeof(char));
	int i;
	for(i = 0; i < mem->total_storage; i++) {
		mem->data[i] = '.';
	}
	mem->free_storage = mem->total_storage;
	mem->most_recent_i = 0;
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

void add_memory_next_fit(memory *mem, char p_id, int p_mem) {
	
}