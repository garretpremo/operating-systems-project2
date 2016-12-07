#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "memory.h"
#include "process.h"

#define FRAMES 32
#define LINES 8

#define t_memmove 1

void test_memory();
void parse(const char *filename, process_list *proclist);

int num_processes = -1;

int main(int argc, char *argv[]) {

	if(argc != 2) {
		printf("ERROR: Invalid arguments\nUSAGE: ./a.out <filename>\n");
		return EXIT_FAILURE;
	}

	process_list proclist;
	proclist.size = 0;

	// parse the input file
	parse(argv[1], &proclist);

	// test
	test_memory();

	// print_process_list(&proclist);
	free_process_list(&proclist);

	return EXIT_SUCCESS;
}

void test_memory() {
	// initialize memory
	memory m;
	init_memory(&m);

	int t = 0;

	m.values = calloc(num_processes, sizeof(process_values));

	print_memory(&m);	

	// test add_memory_next_fit
	add_memory_next_fit(&m, t, 'C', 110);
	add_memory_next_fit(&m, t, 'E', 10);
	add_memory_next_fit(&m, t, 'B', 110);
	add_memory_next_fit(&m, t, 'A', 15);
	add_memory_next_fit(&m, t, 'D', 9);


	// test remove_memory
	remove_memory(&m, 'E');
	remove_memory(&m, 'A');


	// test add worst and best fits
	add_memory_worst_fit(&m, t, 'G', 6);

	add_memory_best_fit(&m, t, 'F', 9);
	add_memory_worst_fit(&m, t, 'F', 9);

	// add_memory_next_fit(&m, 'D', 120);
	// add_memory_next_fit(&m, 'E', 10);


	// test defragment
	defragment_memory(&m);

	print_memory(&m);

	// cleanup
	free_memory(&m);
}

void parse(const char *filename, process_list *proclist) {
	FILE *f;

	f = fopen(filename, "r");
	if(f == NULL) {
		perror("fopen() failed");
		exit(EXIT_FAILURE);
	}


	proclist->size = 0;

	process p;
	new_process(&p);

	size_t num_procs_max_size = 10;
	char *num_procs = (char *)calloc(num_procs_max_size, sizeof(char));
	size_t num_procs_size = 0;

	size_t mem_max_size = 10;
	char *mem = (char *)calloc(mem_max_size, sizeof(char));
	size_t mem_size = 0;

	size_t process_max_size = 10;
	char *process = (char *)calloc(10, sizeof(char));
	size_t process_size = 0;

	bool comment = false;
	bool process_id_found = false;

	char c;

	while((c = getc(f)) != EOF) {
		switch(c) {
			case '#':
				comment = true;
				continue;
			case '\n':
				if(comment)
					comment = false;

				// record number of processes
				else if(num_processes == -1) {
					num_processes = atoi(num_procs);
					proclist->processes = calloc(num_processes, sizeof(process));
				}
				// reset process p after storing it
				else {
					process_id_found = false;

					copy_string_into_process(&p, process, process_size);

					// reset process
					free(process);
					process = (char *)calloc(process_max_size, sizeof(char));
					process_size = 0;

					// copy data over
					copy_process_into_process_list(proclist, &p);

					// reset memory
					free(mem);
					mem = (char *)calloc(mem_max_size, sizeof(char));
					mem_size = 0;
				}
				continue;
			case '\t':
			case ' ':
				if(comment)
					continue;
				else if(!process_id_found) {
					process_id_found = true;
				} else if(p.mem == -1) {
					p.mem = atoi(mem);
				}
				else {
					copy_string_into_process(&p, process, process_size);

					// reset process
					free(process);
					process = (char *)calloc(process_max_size, sizeof(char));
					process_size = 0;
				}
				continue;
			default:
				if(comment)
					continue;
				else if(num_processes == -1) {
					num_procs[num_procs_size] = c;
					num_procs_size++;
					if(num_procs_max_size == num_procs_size) {
						num_procs_max_size = num_procs_max_size*2;
						num_procs = realloc(num_procs, num_procs_max_size*sizeof(char));
					}
				}
				else if(p.id == ' ') {
					p.id = c;
				} else if(p.mem == -1) {
					mem[mem_size] = c;
					mem_size++;
					if(mem_max_size == mem_size) {
						mem_max_size = mem_max_size*2;
						mem = realloc(mem, mem_max_size*sizeof(char));
					}
				}
				else {
					process[process_size] = c;
					process_size++;
					if(process_max_size == process_size) {
						process_max_size = process_max_size*2;
						process = realloc(process, process_max_size*sizeof(char));
					}
					process[process_size] = '\0';
				}
		}
	}
	// get the final recorded process after EOF
	copy_string_into_process(&p, process, process_size);
	copy_process_into_process_list(proclist, &p);

	// cleanup	
	fclose(f);

	free_process(&p);
	free(process);
	free(mem);
	free(num_procs);

}