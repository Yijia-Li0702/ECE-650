#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "my_malloc.h"

void initialize(){
	ifInit = 1;
	memHead = sbrk(0);
	memTail = memHead;
	block * freeList = memstart;
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
	if(ifInit == 0){
		initialize();
		ifInit = 1;
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
	//	从头遍历 find the first 
	block * curr = memHead;
	while(curr->next != NULL){
		if(curr->size = size && curr->available){
			curr->available = 0;
			return curr + sizeof(block);
		} else if(curr->size >= size + sizeof(block) && curr->available){
			curr = split_blk(curr,size);
			return curr + sizeof(block);
		} else {
			curr = curr->next;
		}
	}
	//there's no space qualified, request more memory 
	block * mcb = getNewMem(size+sizeof(block));
	curr->next = mcb;
	memTail = mcb;
	return mcb + sizeof(block);
}

void ff_free(void * ptr){
	block * curr = (block *)ptr - sizeof(block);
	//need assert ?????
	curr->available = 1;
	curr = merge_next(curr);
	curr = merge_prev(curr);
}

void * bf_malloc(size_t size){
	if(ifInit == 0){
		initialize();
		ifInit = 1;
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
		if(curr->size = size && curr->available){
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

block * split_blk(block * curr, size_t size){
	curr->available = 0;
	//how to construct a new block
	block * new_mcb = (block *)curr+sizeof(block)+size;
	new_mcb->available = 1;
	new_mcb -> next = curr->next;
	curr->next = new_mcb;
	new_mcb->size = curr->size-size-sizeof(block);
	curr->size = size;
	if(curr == memTail){
		memTail = new_mcb;
	}
	return curr;
}

void bf_free(void * ptr){
	ff_free(ptr);
}

block * merge_next(block * curr){
	while(curr != memTail){
		block * next_mcb = curr->next;
		//merge with the next block 
		if(next_mcb->available = 1){
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
		if(prev_mcb->available = 1){
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

void * getNewMem(size_t size){
	//find the current location of the program break
	block * new_blk = sbrk(0);
	//if sbrk fails
	if(sbrk(size) == (void*) - 1){
		return NULL;
	}
	new_blk->size = size-sizeof(block);
	new_blk->next = NULL;
	new_blk->available = 0;
	new_blk->prev = memTail;
	memTail = new_blk;
	return new_blk;
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