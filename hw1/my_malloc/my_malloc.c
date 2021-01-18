#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "my_malloc.h"

void initialize(){
	ifInit = 1;
	memHead = sbrk(0);
	memTail = memHead;
	block * freeList = memHead;
	freeList->size = 0;
	freeList->available = 1;
	freeList->next = NULL;
	freeList->prev = NULL;
}

//if there are memory allocated before
//check if there exists appropriate space
//case 1:mcb->size == size 大小正好
//case 2:mcb->size > size + sizeof(block) put data into it and construct a new mcb
//case 3:mcb->size <size or size < * < size + sizeof(block) next
void * ff_malloc(size_t size){
//	if(ifInit == 0){
//		initialize();
//	}
  if(!ifInit){
    block * mcb = getNewMem(size+sizeof(block));
    //printf("sizeof(block)=%ld\n",sizeof(block));
   	if(mcb == NULL){
			return NULL;
		}
    memHead = mcb;
    //memTail = memHead;
    ifInit = 1;
    printf("return %p\n",mcb + sizeof(block));
    return (char*)mcb + sizeof(block);
  }
	if(size == 0){
		return NULL;
	}
	//	从头遍历 find the first 
	block * curr = memHead;
	while(curr != NULL){
     printf("curr addrass %p, size %ld\n", curr, curr->size);
		if(curr->size == size && curr->available){
			curr->available = 0;
      printf("ff_m curr = %p, size = %ld\n",curr, size);
			return (char*)curr + sizeof(block);
		} else if(curr->size >= size + sizeof(block) && curr->available){
			curr = split_blk(curr,size);
			return (char*)curr + sizeof(block);
		} else {
			curr = curr->next;
		}
	}
	//there's no space qualified, request more memory 
	block * mcb = getNewMem(size+sizeof(block));
 printf("getNewMem after address %p, size %ld\n", mcb, size);
 //printf("curr->next = mcb\n");
	//curr->next = mcb;
 //printf("memTail = mcb\n");
	//memTail = mcb;
 printf("return %p\n",mcb + sizeof(block));
	return (char*)mcb + sizeof(block);
}

void ff_free(void * ptr){
  //printf("free ptr = %p\n", ptr);
	block * curr = (block*)((char*)ptr - sizeof(block));
// printf("free curr merge before = %p, size = %ld\n", curr, curr->size);
	curr->available = 1;
 //printf("curr = merge(curr);\n");
  curr = merge(curr);
  printf("free curr = %p, size = %ld\n", curr, curr->size);
}



block * split_blk(block * curr, size_t size){
	curr->available = 0;
	//how to construct a new block
	block * new_mcb = (block *)((char*)curr+sizeof(block)+size);
	new_mcb->available = 1;
 	if(curr == memTail){
		memTail = new_mcb;
	} else{
    curr->next->prev = new_mcb;
  }
	new_mcb -> next = curr->next;
	curr->next = new_mcb;
  new_mcb->prev = curr;
	new_mcb->size = curr->size-size-sizeof(block);
	curr->size = size;
 printf("split curr = %p, size = %ld;\n",curr, curr->size);
 printf("split new_mcb = %p, size = %ld;\n",new_mcb, new_mcb->size);
	return curr;
}

block * merge(block * curr){
  if(curr != memTail){
  printf("block * next_mcb = curr->next;\n");
  	block * next_mcb = curr->next;
  	if(next_mcb->available == 1){
   printf("merge next_mcb = %p, size = %ld\n", next_mcb, next_mcb->size);
		curr->size = curr->size + next_mcb->size + sizeof(block);
		curr->next = next_mcb->next;
		if(next_mcb == memTail){
			memTail = curr;
		}else{
      next_mcb->next->prev = curr;
    }
	}
  }
  if(curr != memHead){
		block * prev_mcb = curr->prev;
		if(prev_mcb->available == 1){
    printf("merge prev_mcb = %p, size = %ld\n", prev_mcb, prev_mcb->size);
			prev_mcb->size = curr->size + prev_mcb->size + sizeof(block);
			prev_mcb->next = curr->next;
      if(curr != memTail){
        curr->next->prev = prev_mcb;
      } else{
        memTail = prev_mcb;
      }
		}
   return prev_mcb;
	}
	return curr;
}

void * getNewMem(size_t size){
	//find the current location of the program break
	//block * new_blk = sbrk(0);
 block * new_blk = sbrk(size);
 printf("getNewMem address %p, size %ld\n", new_blk, size);
	//if sbrk fails
	//if(sbrk(size) == (void*) - 1){
 if(new_blk == (void*) - 1){
		return NULL;
	}
  //printf("new_blk->size = size-sizeof(block)\n");
	new_blk->size = size-sizeof(block);
  //printf("new_blk->next = NULL\n");
	new_blk->next = NULL;
 //printf("available\n");
	new_blk->available = 0;
 //printf("new_blk->prev\n");
	new_blk->prev = memTail;
 if(memTail != NULL){
   memTail->next = new_blk;
 }
 //printf("memTail = new_blk;\n");
	memTail = new_blk;
	return new_blk;
}

void bf_free(void * ptr){
	ff_free(ptr);
}
unsigned long get_largest_free_data_segment_size(){
	block * curr = memHead;
	unsigned long max = 0;
	while(curr != NULL){
		if(curr -> available && curr->size > max){
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
		if(curr -> available){
			total = total + curr->size;
		}
		curr = curr->next;
	}
	return total;
}


block * merge_next(block * curr){
	while(curr != memTail){
		block * next_mcb = curr->next;
		//merge with the next block 
		if(next_mcb->available == 1){
			curr->size = curr->size + next_mcb->size + sizeof(block);
			curr->next = next_mcb->next;
			next_mcb->next->prev = curr;
			if(next_mcb == memTail){
				memTail = curr;
			}

		} else{break;}
		curr = curr->next;
	}
	return curr;
}

block * merge_prev(block * curr){
	while(curr != memHead){
		block * prev_mcb = curr->prev;
		if(prev_mcb->available == 1){
			prev_mcb->size = curr->size + prev_mcb->size + sizeof(block);
			prev_mcb->next = curr->next;
			curr->next->prev = prev_mcb;
			if(curr == memTail){
				memTail = prev_mcb;
			}
		} else{break;}
		curr = prev_mcb;
	}
	return curr;
}

void * bf_malloc(size_t size){
//	if(ifInit == 0){
//		initialize();
//		ifInit = 1;
//	}
  if(!ifInit){
    block * mcb = getNewMem(size+sizeof(block));
    memHead = mcb;
    memTail = memHead;
    ifInit = 1;
    return mcb + sizeof(block);
  }
	if(size == 0){
		return NULL;
	}
	//if there's no memory allocated before
	if(memHead == memTail) {
		block * mcb = getNewMem(size+sizeof(block));
		if(mcb == NULL){
			return NULL;
		}
		mcb->available = 0;
		void * ptr = mcb + sizeof(block);
		return ptr;
	}
	block * curr = memHead;
	block * best = NULL;
	while(curr->next != NULL){
		if(curr->size == size && curr->available){
			curr->available = 0;
			return curr + sizeof(block);
		} else if(curr->size >= size + sizeof(block) && curr->available){
			if(best == NULL){
				best = curr;
			} else{
				if(curr->size < best->size){
					best = curr;
				}
			}
		} 
		curr = curr->next;
	}
	//if there's no block usable
	if(best == NULL){
		block * mcb = getNewMem(size+sizeof(block));
		curr->next = mcb;
		memTail = mcb;
		return mcb + sizeof(block);
	} else{
			best = split_blk(best,size);
			return best + sizeof(block);

	}
}