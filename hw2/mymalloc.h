#include <unistd.h>

typedef struct block_t{
  size_t size;
  int available;
  struct block_t * next;
  struct block_t * prev;
} block;



pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
block * memHead = NULL;
__thread block_t * memHead_tls = NULL;

void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);

block * merge(block * curr);
void * getNewMem(size_t size);
block * split_blk(block * curr, size_t size);
block * remove_from_freelist(block* curr);
void * getNewMem_tls(size_t size);
