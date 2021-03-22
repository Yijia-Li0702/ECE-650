#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "assert.h"

#include "my_malloc.h"




//this is a malloc function using best fit strategy
void * ts_malloc_lock(size_t size){
  pthread_mutex_lock(&lock);
  if(memHead == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    if(mcb == NULL){
      pthread_mutex_unlock(&lock);
      return NULL;
    }
    pthread_mutex_unlock(&lock);
    return (char*)mcb + sizeof(block);
  }
  if(size == 0){
    pthread_mutex_unlock(&lock);
    return NULL;
  }
  block * curr = memHead;
  block * best = NULL;
  while(curr != NULL){
    //if having equal size, can return directly
    //assert(curr->available == 1);
    if(curr->size == size && curr->available){
      curr->available = 0;
      curr = remove_from_freelist(curr);
      best = curr;
      pthread_mutex_unlock(&lock);
      return (char *)curr + sizeof(block);
    } else if(curr->size >= size + sizeof(block) && curr->available){
      //record the available one with the minimum size
      if(best == NULL||curr->size < best->size){
	best = curr;
      }
    }   
    curr = curr->next;
  }
  //if there's no block usable
  if(best == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    pthread_mutex_unlock(&lock);
    return (char*)mcb + sizeof(block);
  } else{
    best = split_blk(best,size);
    pthread_mutex_unlock(&lock);
    return (char*)best + sizeof(block);
  }
}

//this free function is to free allocated memory and put it into freelist
void ts_free_lock(void * ptr){
  pthread_mutex_lock(&lock);
  //calculate corrrect address
  block *curr = (block*)((char*)ptr - sizeof(block));
  curr->available = 1;
  block * trace= memHead;
  //if the freelist is empty
  if(memHead == NULL){
    memHead = curr;
    curr->next = NULL;
    curr->prev=NULL;
    pthread_mutex_unlock(&lock);
    return;
  }
  //if it's in front of head, it will become head
  if((char*)curr<(char*)memHead){
    curr->next= memHead;
    memHead->prev = curr;
    memHead = curr;
    curr=merge(curr);
    pthread_mutex_unlock(&lock);
    return;
  }
  //check each block in the freelist
  while(trace->next!=NULL){
    //assert(trace->available == 1);
    if((char*)curr > (char*)trace && (char*)curr < (char*)trace->next){
      curr->prev = trace;
      curr->next = trace->next;
      trace->next->prev=curr;
      trace->next = curr;
      curr = merge(curr);
      pthread_mutex_unlock(&lock);
      return;
    }
    trace=trace->next;
  }
  //if it's lower than the tail of freelist
  trace->next = curr;
  curr->prev = trace;
  curr->next = NULL;
  curr = merge(curr);
  pthread_mutex_unlock(&lock);
  return
}

//this function is to remove a used block out of freelist, update its previous and next block
block * remove_from_freelist(block* curr){
  if(memHead == curr||curr->prev == NULL){
    memHead = curr->next;
  } else{
    curr->prev->next = curr->next;
  }
  if(curr->next != NULL){
    curr->next->prev=curr->prev;
  }
  return curr;
}
//this function is to split one block into filled one and usable one
block * split_blk(block * curr, size_t size){
  //how to construct a new block
  curr->available = 0;
  block * new_mcb = (block *)((char*)curr+sizeof(block)+size);
  new_mcb->available = 1;
  new_mcb->next = curr->next;
  new_mcb->prev = curr->prev;
  //put the new block into freelist
  if(curr == memHead||curr->prev == NULL){
    memHead = new_mcb;
  } else{
    curr->prev->next = new_mcb;
  }
  if(curr->next != NULL){
    curr->next->prev = new_mcb;
  }
  new_mcb->size = curr->size-size-sizeof(block);
  curr->size = size;
  return curr;
}
//this function is to merge two free blocks and put the new one into freelist
block * merge(block * curr){
  //if the block isn't at the tail of freelist
  if(curr->next != NULL){
    block * next_mcb = curr->next;
    if((char*)next_mcb == (char*)curr+sizeof(block)+curr->size){
      curr->size = curr->size + next_mcb->size + sizeof(block);
      curr->next = next_mcb->next;
      if(next_mcb->next != NULL){
	next_mcb->next->prev = curr;
      }
    }
  }
  //if this block isn't at the top of freelist
  if(curr != memHead){
    block * prev_mcb = curr->prev;
    if((char*)prev_mcb == (char*)curr-sizeof(block)-prev_mcb->size){
      prev_mcb->size = curr->size + prev_mcb->size + sizeof(block);
      prev_mcb->next = curr->next;
      if(curr->next != NULL){
        curr->next->prev = prev_mcb;
      }
      prev_mcb->available = 1;
      return prev_mcb;
    }
  }
  return curr;
}
//this function is to get new memory by calling sbrk()
void * getNewMem(size_t size){
  //find the current location of the program break
  block * new_blk = sbrk(size);
  if(new_blk == (void*) - 1){
    return NULL;
  }
  new_blk->size = size-sizeof(block);
  new_blk->next = NULL;
  new_blk->available = 0;
  new_blk->prev = NULL;
  return new_blk;
}

+++++++++++++++++++++++++++++++++
void * ts_malloc_nolock(size_t size){
  if(memHead_tls == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    if(mcb == NULL){
      return NULL;
    }
    return (char*)mcb + sizeof(block);
  }
  if(size == 0){
    return NULL;
  }
  block * curr = memHead_tls;
  block * best = NULL;
  while(curr != NULL){
    //if having equal size, can return directly
    //assert(curr->available == 1);
    if(curr->size == size && curr->available){
      curr->available = 0;
      curr = remove_from_freelist(curr);
      best = curr;
      return (char *)curr + sizeof(block);
    } else if(curr->size >= size + sizeof(block) && curr->available){
      //record the available one with the minimum size
      if(best == NULL||curr->size < best->size){
	best = curr;
      }
    }   
    curr = curr->next;
  }
  //if there's no block usable
  if(best == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    return (char*)mcb + sizeof(block);
  } else{
    best = split_blk(best,size);
    return (char*)best + sizeof(block);
  }
}

//this free function is to free allocated memory and put it into freelist
void ts_free_nolock(void * ptr){
  //calculate corrrect address
  block *curr = (block*)((char*)ptr - sizeof(block));
  curr->available = 1;
  block * trace= memHead_tls;
  //if the freelist is empty
  if(memHead_tls == NULL){
    memHead_tls = curr;
    curr->next = NULL;
    curr->prev=NULL;
    return;
  }
  //if it's in front of head, it will become head
  if((char*)curr<(char*)memHead_tls){
    curr->next= memHead_tls;
    memHead_tls->prev = curr;
    memHead_tls = curr;
    curr=merge(curr);
    return;
  }
  //check each block in the freelist
  while(trace->next!=NULL){
    //assert(trace->available == 1);
    if((char*)curr > (char*)trace && (char*)curr < (char*)trace->next){
      curr->prev = trace;
      curr->next = trace->next;
      trace->next->prev=curr;
      trace->next = curr;
      curr = merge(curr);
      return;
    }
    trace=trace->next;
  }
  //if it's lower than the tail of freelist
  trace->next = curr;
  curr->prev = trace;
  curr->next = NULL;
  curr = merge(curr);
  return;
}

void * getNewMem_tls(size_t size){
  //find the current location of the program break
  pthread_mutex_lock(&lock);
  block * new_blk = sbrk(size);
  if(new_blk == (void*) - 1){
    return NULL;
  }
  pthread_mutex_unlock(&lock);
  new_blk->size = size-sizeof(block);
  new_blk->next = NULL;
  new_blk->available = 0;
  new_blk->prev = NULL;
  return new_blk;
}


