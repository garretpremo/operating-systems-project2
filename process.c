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
	p->time = calloc(2, sizeof(times));
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
	p->time = realloc(p->time, q->num_procs*sizeof(times));
	int i;
	for(i = 0; i < q->num_procs; i++) {
		new_times(&(p->time[i]), q->time[i].start, q->time[i].end);
	}
	p->num_procs = q->num_procs;
	p->mem = q->mem;
	p->id = q->id;
}

void copy_string_into_process(process *p, const char *string, const size_t size) {
	int start, i;
	char buff[size];
	for(i = 0; i < size; i++) {
		if(string[i] == '/' && buff != NULL) {
			buff[i] = '\0';
			start = atoi(buff);
			break;
		}
		buff[i] = string[i];
	}

	new_times(&(p->time[p->num_procs]), start, atoi(string+i+1));

	printf("just added start/end time: %d/%d\n", p->time[p->num_procs].start, p->time[p->num_procs].end);

	p->num_procs++;

	p->time = realloc(p->time, (p->num_procs+1)*sizeof(times));
}

void free_process(process *p) {
	free(p->time);
}

void new_times(times *t, int start, int end) {
	t->start = start;
	t->end = end;
}

void print_process_list(process_list *proclist) {
	int i;
	for(i = 0; i < proclist->size; i++) {
		print_process(&(proclist->processes[i]));
	}
}

void print_process(process *p) {
	int i;
	printf("process %c: %d bytes; strt/end times", p->id, p->mem);
	for(i = 0; i < p->num_procs; i++) {
		printf(" %d/%d", p->time[i].start, p->time[i].end);
	}
	printf("\n");
}