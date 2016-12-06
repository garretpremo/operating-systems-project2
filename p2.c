#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "process.h"

#define FRAMES 32
#define LINES 8
#define PROC_LENGTH 10

void parse(const char *filename, process_list *proclist);

int num_processes = -1;

int main(int argc, char *argv[]) {

	if(argc != 2) {
		printf("ERROR: Invalid arguments\nUSAGE: ./a.out <filename>\n");
		return EXIT_FAILURE;
	}

	process_list proclist;
	proclist.size = 0;

	parse(argv[1], &proclist);

	print_process_list(&proclist);
	free_process_list(&proclist);

	return EXIT_SUCCESS;
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

	char *num_procs = (char *)calloc(1, sizeof(char));
	size_t num_procs_size = 0;

	char *mem = (char *)calloc(1, sizeof(char));
	size_t mem_size = 0;

	char *process = (char *)calloc(1, sizeof(char));
	size_t process_size = 0;

	bool comment = false;
	bool process_id_found = false;

	char c;

	while((c = getc(f)) != EOF) {
		printf("%c",c);
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

					copy_string_into_process(&p, &process, &process_size);

					// copy data over
					copy_process_into_process_list(proclist, &p);

					free(mem);
					mem = (char *)calloc(1, sizeof(char));
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
					copy_string_into_process(&p, &process, &process_size);
				}
				continue;
			default:
				if(comment)
					continue;
				else if(num_processes == -1) {
					num_procs[num_procs_size] = c;
					num_procs_size++;
					num_procs = realloc(num_procs, ((num_procs_size+2)*sizeof(char)));
					num_procs[num_procs_size+2] = '\0';
				}
				else if(p.id == ' ') {
					p.id = c;
				} else if(p.mem == -1) {
					mem[mem_size] = c;
					mem_size++;
					mem = realloc(mem, (mem_size+2)*sizeof(char));
					mem[mem_size+2] = '\0';
				}
				else {
					process[process_size] = c;
					process_size++;
					process = realloc(process, ((process_size+2)*sizeof(char)));
					process[process_size+2] = '\0';
				}
		}
	}
	// get the final recorded process after EOF
	copy_string_into_process(&p, &process, &process_size);
	copy_process_into_process_list(proclist, &p);

	printf("\n");

	// cleanup	
	fclose(f);

	free_process(&p);
	free(process);
	free(mem);
	free(num_procs);

}