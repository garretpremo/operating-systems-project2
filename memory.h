#ifndef MEMORY_H_
#define MEMORY_H_


/*	1234..........................32
	================================
1	AAAAAAAABBBBBBBBBBBBBBBBBBBBBBBB
2	BBBBBBBBBBBBBBB.................
3	...................DDDDDDDDD....
4	................................
5	................................
6	........HHHHHHHHHHHHHHHHHHHHHHHH
7	HHH.............................
8	................................
	================================
*/

#define TOTAL_MEMORY 256
#define LINE_SIZE 32

typedef struct memory memory;
typedef struct process_vals process_values;

struct memory {
	char *data;
	process_values *values;
	int stored_procs;
	int total_storage;
	int free_storage;
	int most_recent_i;
};

struct process_vals {
	int start;
	int size;
	char id;
};

void init_memory(memory *mem);
void free_memory(memory *mem);
void copy_memory(memory *mem, const char p_id, const int start, const int end);
void remove_memory(memory *mem, const char p_id);

int memory_used(memory *mem, const char p_id);
int memory_start(memory *mem, const char p_id);
int memory_end(memory *mem, const char p_id);
bool enough_memory(const memory *mem, const int p_mem);

/* defragmentation */
void defragment_memory(memory *mem);

/* algorithms */
void add_memory_next_fit(memory *mem, int t, const char p_id, const int p_mem);
void add_memory_best_fit(memory *mem, int t, const char p_id, const int p_mem);
void add_memory_worst_fit(memory *mem, int t, const char p_id, const int p_mem);

void print_memory(memory *mem);

void get_value(memory *mem, const char p_id, process_values *pval);
void init_value(process_values *pval, const char p_id, const int start, const int size);
void edit_value(memory *mem, const char p_id, const int start, const int size);
void print_value(const process_values *pval);
void print_values(const memory *mem);
void remove_value(memory *mem, const char p_id);

#endif //MEMORY_H_