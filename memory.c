#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include "memory.h"
#include "util.h"

#define t_memmove 1

pthread_mutex_t mutex;

void init_memory(memory *mem, const int process_count) {
	mem->data = (char *)calloc(TOTAL_MEMORY, sizeof(char));
	memset(mem->data, '.', TOTAL_MEMORY);
	mem->values = calloc(process_count, sizeof(process_values));
	mem->stored_procs = 0;
	mem->total_storage = TOTAL_MEMORY;
	mem->free_storage = mem->total_storage;
	mem->most_recent_i = 0;
}

void free_memory(memory *mem) {
	free(mem->data);
	free(mem->values);
}

/* 	copies process ID into memory from start to end.

	this function assumes the partition between start and
		end is ALL free memory
*/
void copy_memory(memory *mem, const char p_id, const int start, const int end) {
	memset(mem->data+start, p_id, end-start);
}

/*	removes process from memory  */
void remove_memory(memory *mem, const char p_id) {
	process_values value;
	get_value(mem, p_id, &value);
	if(value.id == p_id && value.start >= 0 && value.size >= 0) {
		copy_memory(mem, '.', value.start, value.start + value.size);
		mem->free_storage += value.size;

		remove_value(mem, p_id);
		printf("removed %d memory units; %d total remaining\n", value.size, mem->free_storage);
	}
}

/* 	returns the amount of memory a process is using  */
int memory_used(memory *mem, const char p_id) {
	process_values value;
	get_value(mem, p_id, &value);
	if(value.size)
		return value.size;
	return 0;
}

/* 	returns the first place of the process  */
int memory_start(memory *mem, const char p_id) {
	process_values value;
	get_value(mem, p_id, &value);
	if(value.start)
		return value.start;
	return 0;
}

/*	returns the last place of the process  */
int memory_end(memory *mem, const char p_id) {
	return memory_start(mem, p_id)+memory_used(mem, p_id)-1;
}

/*  returns true if there is enough free storage for the process  */
bool enough_memory(const memory *mem, const int p_mem) {
	return p_mem <= mem->free_storage;
}

/*	defragments memory  

	return: the amount of time it took to defragment
*/
int defragment_memory(memory *mem) {
	pthread_mutex_lock(&mutex);
	int i;
	int blank_space = 0;
	int moved_frames = 0;
	int max_end = 0;
	for(i = 0; i < mem->total_storage; i++) {
		if(mem->data[i] == '.')
			blank_space++;
		else if(blank_space != 0 ) {
			int start = i - blank_space;
			int used = memory_used(mem, mem->data[i]);

			copy_memory(mem, mem->data[i], start, start + used);
			edit_value(mem, mem->data[i], start, start + used);
			max_end = max(max_end, start + used);
			
			copy_memory(mem, '.', start + used, i+used);
			i = start + used;
			i--;
			moved_frames += blank_space;
			blank_space = 0;
		}
	}
	mem->most_recent_i = max_end;
	pthread_mutex_unlock(&mutex);
	return moved_frames * t_memmove;
}

/* 	next fit memory algorithm. process is placed in the first
		free partition available.

	memory: the data structure containing memory
	p_id: the id of the process that needs to store memory
	p_mem: the amount of memory the process needs
	return: the amount of time it took to add the memory
*/
int add_memory_next_fit(memory *mem, int t, const char p_id, const int p_mem) {
	int scanned = 0;
	int free_mem = 0;
	int max_partition_size = 0;
	bool stored = false;

	printf("time %dms: Process %c arrived (requires %d frames)\n", t, p_id, p_mem);

	if(!enough_memory(mem, p_mem)) {
		printf("time %dms: not enough storage for %c, moving to next process\n", t, p_id);
		return 0;
	}

	int i;
	for(i = mem->most_recent_i; i < mem->total_storage && scanned < mem->total_storage; i++, scanned++) {
		if(free_mem == p_mem && !stored) {
			stored = true;
			copy_memory(mem, p_id, i-p_mem, i);
			mem->most_recent_i = i;
			mem->free_storage -= p_mem;
			init_value(&(mem->values[mem->stored_procs]), p_id, i-p_mem, p_mem);
			mem->stored_procs++;
		}
		if(mem->data[i] == '.') { 
			free_mem++; 
			
		} else {
			max_partition_size = max(free_mem, max_partition_size);
			free_mem = 0;
		}
		if(i+1 == mem->total_storage) {
			i = -1;
			free_mem = 0;
		}
	}
	if(stored) {
		printf("time %dms: Placed process %c:\n", t, p_id);
		print_memory(mem);
		return 0;
	}
	else {
		printf("time %dms: no sufficient partition size; defragmenting\n", t);
		int defrag_time = defragment_memory(mem);
		return defrag_time + add_memory_next_fit(mem, t, p_id, p_mem);
	}
}

/*	best fit memory algorithm. process is placed in the smallest
		free partition available
	
	memory: the data structure containing memory
	p_id: the id of the process that needs to store memory
	p_mem: the amount of memory the process needs
*/
void add_memory_best_fit(memory *mem, int t, const char p_id, const int p_mem) {
	int scanned = 0;
	int free_mem = 0;
	int start = -1;
	int min_partition_size = mem->total_storage;

	printf("time %dms: Process %c arrived (requires %d frames)\n", t, p_id, p_mem);

	if(!enough_memory(mem, p_mem)) {
		printf("time %dms: not enough storage for %c, moving to next process\n", t, p_id);
		return;
	}

	int i;
	for(i = mem->most_recent_i; i < mem->total_storage && scanned < mem->total_storage; i++, scanned++) {
		if(mem->data[i] == '.') { 
			free_mem++; 
		}
		if(mem->data[i] != '.' || i+1 == mem->total_storage) {
			int temp = min_partition_size;

			if(free_mem >= p_mem) {
				min_partition_size = min(free_mem, min_partition_size);

				if(min_partition_size < temp || start < 0){
					start = (i - min_partition_size);
				}
				else if (free_mem == temp && temp != mem->total_storage){
					start = min(start, i - min_partition_size);
				}
			}
			free_mem = 0;
			if(i+1 == mem->total_storage) {
				i = -1;
			}
		}
	}

	if(start >= 0 && p_mem <= min_partition_size) {

		copy_memory(mem, p_id, start, start + p_mem);
		mem->most_recent_i = start+p_mem;
		mem->free_storage -= p_mem;
		init_value(&(mem->values[mem->stored_procs]), p_id, start, p_mem);
		mem->stored_procs++;

		printf("time %dms: Placed process %c:\n", t, p_id);
		print_memory(mem);
	}
	else if(min_partition_size < p_mem || min_partition_size == mem->total_storage) {
		printf("time %dms: No sufficient partition size; defragmenting\n", t);
		defragment_memory(mem);
		add_memory_best_fit(mem, t, p_id, p_mem);
	}
}

/*	worst fit memory algorithm. process is placed in the largest
		free partition available that it can fit
	
	memory: the data structure containing memory
	p_id: the id of the process that needs to store memory
	p_mem: the amount of memory the process needs
*/
void add_memory_worst_fit(memory *mem, int t, const char p_id, const int p_mem) {
	int scanned = 0;

	// consecutive free memory
	int free_mem = 0;

	int start = -1;
	int max_partition_size = 0;


	printf("time %dms: Process %c arrived (requires %d frames)\n", t, p_id, p_mem);

	// ensure we have enough available memory to store this process
	if(!enough_memory(mem, p_mem)) {
		printf("time %dms: not enough storage for %c, moving to next process\n", t,p_id);
		return;
	}

	int i;

	// look for where to place memory
	for(i = mem->most_recent_i; i < mem->total_storage && scanned < mem->total_storage; i++, scanned++) {
		// record consecutive free memory slots
		if(mem->data[i] == '.') { 
			free_mem++; 
		}
		// not free memory or end of memory
		if(mem->data[i] != '.' || i+1 == mem->total_storage) {
			int temp = max_partition_size;
			if(free_mem >= p_mem) {
				max_partition_size = max(free_mem, max_partition_size);

				// set starting point of where the memory should be placed
				if(max_partition_size > temp || start < 0)
					start = i - max_partition_size;
				else if (free_mem == temp && free_mem > 0) {
					start = min(start, i - max_partition_size);
				}
			}
			free_mem = 0;

			// go back to the beginning if we're at the end
			// and haven't seen every element
			if(i+1 == mem->total_storage)
				i = -1;
		}
	}
	// found a starting point and a big enough partition size
	if(start >= 0 && p_mem <= max_partition_size) {

		// copy the memory into the partition
		copy_memory(mem, p_id, start, start + p_mem);

		// record the most recently stored data index
		mem->most_recent_i = start+p_mem;

		// record that there's less total free storage
		mem->free_storage -= p_mem;

		// record the id, start, and size values of the proc
		init_value(&(mem->values[mem->stored_procs]), p_id, start, p_mem);

		// increment the total number of processes in memory
		mem->stored_procs++;

		printf("time %dms: Placed process %c:\n", t, p_id);
		print_memory(mem);
	}
	else {
		printf("no sufficient partition size; defragmenting\n");
		defragment_memory(mem);
		add_memory_best_fit(mem, t, p_id, p_mem);
	}
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

void init_value(process_values *pval, const char p_id, const int start, const int size) {
	pval->start = start;
	pval->size = size;
	pval->id = p_id;
}

void edit_value(memory *mem, const char p_id, const int start, const int size) {
	process_values pval;
	get_value(mem, p_id, &pval);

	if(pval.id == p_id) {
		pval.start = start;
		pval.size = start;
	}
}

void get_value(memory *mem, const char p_id, process_values *pval) {
	int i;
	bool found = false;
	for(i = 0; i < mem->stored_procs; i++) {
		if(mem->values[i].id == p_id) {
			init_value(pval, p_id, mem->values[i].start, mem->values[i].size);
			return;
		}
	}
	if(!found)
		init_value(pval, '\0', -1, -1);
}

void print_value(const process_values *pval) {
	printf("%c: starts at %d, with a size of %d\n", pval->id, pval->start, pval->size);
}

void print_values(const memory *mem) {
	int i;
	printf("print values\n");
	for(i = 0; i < mem->stored_procs; i++) {
		print_value(&(mem->values[i]));
	}
}

void remove_value(memory *mem, const char p_id) {
	bool found = false;
	int i;
	for(i = 0; i < mem->stored_procs; i++) {
		if(mem->values[i].id == p_id) {
			found = true;
		}
		if(found && i < mem->stored_procs-1) {
			mem->values[i] = mem->values[i+1];
		} 
		else if(i == mem->stored_procs-1) {
			init_value(&(mem->values[i]), ' ', 0, 0);
		}
	}
	if(found)
		mem->stored_procs -= 1;
}