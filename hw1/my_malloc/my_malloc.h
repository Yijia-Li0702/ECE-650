#include <unistd.h>

typedef struct block_t{
  size_t size;
  int available;
  struct block_t * next;
  struct block_t * prev;
} block;

block * memHead = NULL;

//First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void * ptr);
//Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void * ptr);
block * merge(block * curr);
void * getNewMem(size_t size);
block * split_blk(block * curr, size_t size);
block * remove_from_freelist(block* curr);

unsigned long get_largest_free_data_segment_size();//in bytes
unsigned long get_total_free_size();//in bytes