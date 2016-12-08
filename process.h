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
	int start;
	int end;
	int mem;
	char id;
};

void new_process_list(process_list *proclist);
void free_process_list(process_list *proclist);

void new_process(process *p);
void copy_process_into_process_list(process_list *proclist, process *p);
void copy_process(process *p, process *q);
void free_process(process *p);

void copy_string_into_process(process *p, const char *string, const size_t size);

void print_process_list(process_list *proclist);
void print_process(process *process);

void sort_process_list(process_list *proclist);

int compare_process_by_start_then_id(const void * a, const void * b);

#endif //PROCESS_H_