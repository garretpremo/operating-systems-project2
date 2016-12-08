#ifndef PROCESS_H_
#define PROCESS_H_

typedef struct process_list process_list;
typedef struct process process;
typedef struct times times;

struct process_list {
	process *processes;
	int size;
};

struct process {
	times *time;
	int num_procs;
	int mem;
	char id;
};

struct times {
	int start;
	int end;
};

void new_process_list(process_list *proclist);
void free_process_list(process_list *proclist);

void new_process(process *p);
void copy_process_into_process_list(process_list *proclist, process *p);
void copy_process(process *p, process *q);
void free_process(process *p);

void new_times(times *t, int start, int end);

void copy_string_into_process(process *p, const char *string, const size_t size);

void print_process_list(process_list *proclist);
void print_process(process *process);

#endif //PROCESS_H_