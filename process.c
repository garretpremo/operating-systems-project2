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
	free(proclist->processes);
}

void new_process(process *p) {
	p->start = -1;
	p->end = -1;
	p->mem = -1;
	p->id = ' ';
}

void copy_process_into_process_list(process_list *proclist, process *p) {
	copy_process(&(proclist->processes[proclist->size]), p);

	proclist->size++;
}

void copy_process(process *p, process *q) {
	p->start = q->start;
	p->end = q->end;
	p->mem = q->mem;
	p->id = q->id;
}

void copy_string_into_process(process *p, const char *string, const size_t size) {
	int i;
	char buff[size];
	for(i = 0; i < size; i++) {
		if(string[i] == '/' && buff != NULL) {
			buff[i] = '\0';
			p->start = atoi(buff);
			p->end = atoi(string+i+1);
			break;
		}
		buff[i] = string[i];
	}
}

void print_process_list(process_list *proclist) {
	int i;
	for(i = 0; i < proclist->size; i++) {
		print_process(&(proclist->processes[i]));
	}
}

void print_process(process *p) {
	printf("process %c: %d bytes; %d/%d time\n", p->id, p->mem, p->start, p->end);
}


int compare_process_by_start_then_id(const void * a, const void * b) {
	if((*(process*)a).start < (*(process*)b).start) {
		return -1;
	}
	else if ((*(process*)a).start > (*(process*)b).start) {
		return 1;
	}
	else {
		if((*(process*)a).id <= (*(process*)b).id) {
			return -1;
		}
		else 
			return 1;
	}
}