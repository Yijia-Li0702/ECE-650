#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "assert.h"

#include "my_malloc.h"

//malloc() function using first fit strategy
void * ff_malloc(size_t size){
  //freelist is empty
  if(memHead == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    if(mcb == NULL){
      return NULL;
    }
    return (char*)mcb + sizeof(block);
  }
  if(size == 0){
    return NULL;
  } 
  //check each free block, find eligible one
  block * curr = memHead;
  while(curr != NULL){
    //block has exactly the same size
    if(curr->size == size){
      curr = remove_from_freelist(curr);
      return (char*)curr + sizeof(block);
    } else if(curr->size >= size + sizeof(block)){
        //if the size is enough, split the block
        curr = split_blk(curr,size);
        return (char*)curr + sizeof(block);
    } else {
        curr = curr->next;
    }
  }
  //if there's no block eligible, get a new one
  block * mcb = getNewMem(size+sizeof(block));
  return (char*)mcb + sizeof(block);
}

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

//this is malloc function using best fit strategy
void * bf_malloc(size_t size){
  if(memHead == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    if(mcb == NULL){
      return NULL;
    }
    return (char*)mcb + sizeof(block);
  }
  if(size == 0){
    return NULL;
  }
  block * curr = memHead;
  block * best = NULL;
  while(curr != NULL){
    //if having equal size, can return directly
    if(curr->size == size){
      curr = remove_from_freelist(curr);
      best = curr;
      return (char *)curr + sizeof(block);
    } else if(curr->size >= size + sizeof(block)){
      //record the available one with the minimum size
      if(best == NULL||curr->size < best->size){
	      best = curr;
      }
    }   
    curr = curr->next;
  }
  if(best == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    return (char*)mcb + sizeof(block);
  } else{
    best = split_blk(best,size);
    return (char*)best + sizeof(block);
  }
}
//free function to free allocated memory and put it into  freelist
void ff_free(void * ptr){
  //calculate corrrect address
  block *curr = (block*)((char*)ptr - sizeof(block));
  block * trace= memHead;
  //if the freelist is empty
  if(memHead == NULL){
    memHead = curr;
    curr->next = NULL;
    curr->prev=NULL;
    return;
  }
  //if it's in front of head, it will become head
  if((char*)curr<(char*)memHead){
    curr->next= memHead;
    memHead->prev = curr;
    memHead = curr;
    curr=merge(curr);
    return;
  }
  //check each block in the freelist
  while(trace->next!=NULL){
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
}

//this function is to split one block into filled one and usable one
block * split_blk(block * curr, size_t size){
  //how to construct a new block
  block * new_mcb = (block *)((char*)curr+sizeof(block)+size);
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
  if(curr->next!=NULL){
    block * next_mcb = curr->next;
    if((char*)next_mcb == (char*)curr+sizeof(block)+curr->size){
      curr->size = curr->size + next_mcb->size + sizeof(block);
      curr->next = next_mcb->next;
      if(next_mcb->next != NULL){
	      next_mcb->next->prev = curr;
      }
    }
  }
  if(curr != memHead){
    block * prev_mcb = curr->prev;
    if((char*)prev_mcb == (char*)curr-sizeof(block)-prev_mcb->size){
      prev_mcb->size = curr->size + prev_mcb->size + sizeof(block);
      prev_mcb->next = curr->next;
      if(curr->next != NULL){
        curr->next->prev = prev_mcb;
      }
      return prev_mcb;
    }
  }
  return curr;
}

void * getNewMem(size_t size){
  //find the current location of the program break
  block * new_blk = sbrk(size);
  if(new_blk == (void*) - 1){
    return NULL;
  }
  new_blk->size = size-sizeof(block);
  new_blk->next = NULL;
  new_blk->prev = NULL;
  return new_blk;
}

void bf_free(void * ptr){
  ff_free(ptr);
}
unsigned long get_largest_free_data_segment_size(){
  block * curr = memHead;
  unsigned long max = 0;
  while(curr != NULL){
    if(curr->size > max){
      max = curr->size;
    }
    curr = curr->next;
  }
  return max;
}

unsigned long get_total_free_size(){
  block * curr = memHead;
  unsigned long total = 0;
  while(curr != NULL){
    total = total + curr->size;    
    curr = curr->next;
  }
  return total;
}

void print(){  
  block* curr=memHead;
  if(curr->prev!= NULL){
    printf("error!");
  }
  while(curr != NULL){
    printf("freelist address = %p, size = %ld\n", curr, curr->size);
    curr = curr->next;
  }
}


