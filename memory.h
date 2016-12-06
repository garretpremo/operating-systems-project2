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

struct memory {
	char *data;
	int total_storage;
	int free_storage;
	int most_recent_i;
};

void init_memory(memory *mem);

void copy_memory(memory *mem, const char p_id, const int start, const int end);

/* algorithms */
void add_memory_next_fit(memory *mem, const char p_id, const int p_mem);
void add_memory_best_fit(memory *mem, const char p_id, const int p_mem);
void add_memory_worst_fit(memory *mem, const char p_id, const int p_mem);

void print_memory(memory *mem);

#endif //MEMORY_H_