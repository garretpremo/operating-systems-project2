#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "process.h"

void new_process_list(process_list *proclist) {
	proclist->processes = calloc(1, sizeof(process));
	proclist->size = 0;
}

void free_process_list(process_list *proclist) {
	int i;
	for(i = 0; i < proclist->size; i++) {
		free_process(&(proclist->processes[i]));
	}
	free(proclist->processes);
}

void new_process(process *p) {
	p->processes = (char **)calloc(2, sizeof(char *));
	p->num_procs = 0;
	p->mem = -1;
	p->id = ' ';
}

void copy_process_into_process_list(process_list *proclist, process *p) {
	new_process(&(proclist->processes[proclist->size]));
	copy_process(&(proclist->processes[proclist->size]), p);

	free_process(p);
	new_process(p);

	proclist->size++;
}

void copy_process(process *p, process *q) {
	p->processes = (char **)realloc(p->processes, q->num_procs*sizeof(char *));
	int i;
	for(i = 0; i < q->num_procs; i++) {
		p->processes[i] = (char *)calloc(strlen(q->processes[i])+1, sizeof(char));
		memcpy((p->processes[i]), q->processes[i], strlen(q->processes[i])+1);
	}
	p->num_procs = q->num_procs;
	p->mem = q->mem;
	p->id = q->id;
}

void copy_string_into_process(process *p, const char *string, const size_t size) {
	p->processes[p->num_procs] = (char *)calloc(size+1, sizeof(char));

	memcpy((p->processes[p->num_procs]), string, size+1);

	p->num_procs++;

	p->processes = realloc(p->processes, (p->num_procs+1)*sizeof(char *));
}

void free_process(process *p) {
	int i;
	for(i = 0; i < p->num_procs; i++) {
		free(p->processes[i]);
	}
	free(p->processes);
}

void print_process_list(process_list *proclist) {
	int i, j;
	for(i = 0; i < proclist->size; i++) {
		printf("process %c: %d bytes;", proclist->processes[i].id, proclist->processes[i].mem);
		for(j = 0; j < proclist->processes[i].num_procs; j++) {
			printf(" %s", proclist->processes[i].processes[j]);
		}
		printf(".\n");
	}
}