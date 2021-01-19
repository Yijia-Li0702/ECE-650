#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "my_malloc.h"


//if there are memory allocated before
//check if there exists appropriate space
//case 1:mcb->size == size 大小正好
//case 2:mcb->size > size + sizeof(block) put data into it and construct a new mcb
//case 3:mcb->size <size or size < * < size + sizeof(block) next
void * ff_malloc(size_t size){
  if(memHead == NULL){
    block * mcb = getNewMem(size+sizeof(block));
    printf("getNew mcb = %p, size = %ld\n",mcb, size);
   	if(mcb == NULL){
			return NULL;
		}
//    memHead = mcb;
//    ifInit = 1;
  //printf("return %p\n",mcb + sizeof(block));
    return (char*)mcb + sizeof(block);
  }
	if(size == 0){
		return NULL;
	}
	//	从头遍历 find the first 
	block * curr = memHead;
	while(curr != NULL){
     //printf("curr addrass %p, size %ld\n", curr, curr->size);
		if(curr->size == size && curr->available){
			  curr->available = 0;
       if(memHead != curr){
         curr->prev->next = curr->next;
       } else{
         memHead = curr->next;
       }
       if(memTail!= curr){
         curr->next->prev=curr->prev;
       } else{
         memTail = curr->prev;
       }
      printf("ff_m curr = %p, size = %ld\n",curr, size);
			return (char*)curr + sizeof(block);
		} else if(curr->size >= size + sizeof(block) && curr->available){
			curr = split_blk(curr,size);
      printf("ff_m curr = %p, size = %ld\n",curr, size);
			return (char*)curr + sizeof(block);
		} else {
			curr = curr->next;
		}
	}
	//there's no space qualified, request more memory 
	block * mcb = getNewMem(size+sizeof(block));
  printf("getNewMem after address %p, size %ld\n", mcb, size);
	return (char*)mcb + sizeof(block);
}

void ff_free(void * ptr){
  //printf("free ptr = %p\n", ptr);
	block *curr = (block*)((char*)ptr - sizeof(block));
  //printf("free curr merge before = %p, size = %ld\n", curr, curr->size);
	curr->available = 1;
 block *  trace= memHead;
 if(memHead == NULL){
     memHead = curr;
     memTail = curr;
     curr->next = NULL;
     curr->prev=NULL;
     printf("free curr = %p, size = %ld\n", curr, curr->size);
     return;
 }
 if((char*)curr<(char*)memHead){
   curr->next= memHead;
   memHead->prev = curr;
   memHead = curr;
   curr=merge(curr);
   printf("free curr = %p, size = %ld\n", curr, curr->size);
   return;
 }
 if((char*)curr>(char*)memTail){
     curr->prev = memTail;
     memTail->next = curr;
     memTail = curr;
     curr = merge(curr);
     printf("free curr = %p, size = %ld\n", curr, curr->size); 
     return;
 }
 while(trace->next!=NULL){
   if((char*)curr > (char*)trace && (char*)curr < (char*)trace->next){
     curr->prev = trace;
     curr->next = trace->next;
     trace->next->prev=curr;
     trace->next = curr;
     curr = merge(curr);
     printf("free curr = %p, size = %ld\n", curr, curr->size);
     return;
   }
   trace=trace->next;
 }
  //printf("free curr = %p, size = %ld\n", curr, curr->size);
}

block * split_blk(block * curr, size_t size){
	curr->available = 0;
	//how to construct a new block
	block * new_mcb = (block *)((char*)curr+sizeof(block)+size);
	new_mcb->available = 1;
 new_mcb->next = curr->next;
 new_mcb->prev = curr->prev;
 if(curr == memHead){
   memHead = new_mcb;
 } else{
   curr->prev->next = new_mcb;
 }
 	if(curr == memTail){
		memTail = new_mcb;
	} else{
    curr->next->prev = new_mcb;
  }
	new_mcb -> next = curr->next;
	//curr->next = new_mcb;
	new_mcb->size = curr->size-size-sizeof(block);
	curr->size = size;
   printf("split curr = %p, size = %ld;\n",curr, curr->size);
   printf("split new_mcb = %p, size = %ld;\n",new_mcb, new_mcb->size);
	return curr;
}

block * merge(block * curr){
  if(curr != memTail){
  	block * next_mcb = curr->next;
     printf("(char*)next_mcb add = %p\n",(char*)next_mcb);
      printf("(char*)curr+sizeof(block) = %p\n",(char*)curr+sizeof(block));
      printf("next_mcb add = %p\n",next_mcb);
      printf("(block*)(char*)curr+sizeof(block) = %p\n",(block*)((char*)curr+sizeof(block)));
  	if((char*)next_mcb == (char*)curr+sizeof(block)+curr->size){
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
     printf("prev_mcb add = %p\n",(char*)prev_mcb);
  printf("(char*)curr-sizeof(block) = %p\n",(char*)curr-sizeof(block)-prev_mcb->size);
		if((char*)prev_mcb == (char*)curr-sizeof(block)-prev_mcb->size){
    printf("merge prev_mcb = %p, size = %ld\n", prev_mcb, prev_mcb->size);
			prev_mcb->size = curr->size + prev_mcb->size + sizeof(block);
			prev_mcb->next = curr->next;
      if(curr != memTail){
        curr->next->prev = prev_mcb;
      } else{
        memTail = prev_mcb;
      }
       return prev_mcb;
		}
  
	}
	return curr;
}

void * getNewMem(size_t size){
	//find the current location of the program break
 block * new_blk = sbrk(size);
 //printf("getNewMem address %p, size %ld\n", new_blk, size);
 if(new_blk == (void*) - 1){
		return NULL;
	}
	new_blk->size = size-sizeof(block);
	new_blk->next = NULL;
	new_blk->available = 0;
	new_blk->prev = NULL;
// if(memTail != NULL){
//   memTail->next = new_blk;
// }
//	memTail = new_blk;
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

void print(){  
 block* curr=memHead;
  while(curr != NULL){
    printf("freelist address = %p, size = %ld\n", curr, curr->size);
    curr = curr->next;
  }
}


//block * merge_next(block * curr){
//	while(curr != memTail){
//		block * next_mcb = curr->next;
//		//merge with the next block 
//		if(next_mcb->available == 1){
//			curr->size = curr->size + next_mcb->size + sizeof(block);
//			curr->next = next_mcb->next;
//			next_mcb->next->prev = curr;
//			if(next_mcb == memTail){
//				memTail = curr;
//			}
//
//		} else{break;}
//		curr = curr->next;
//	}
//	return curr;
//}
//
//block * merge_prev(block * curr){
//	while(curr != memHead){
//		block * prev_mcb = curr->prev;
//		if(prev_mcb->available == 1){
//			prev_mcb->size = curr->size + prev_mcb->size + sizeof(block);
//			prev_mcb->next = curr->next;
//			curr->next->prev = prev_mcb;
//			if(curr == memTail){
//				memTail = prev_mcb;
//			}
//		} else{break;}
//		curr = prev_mcb;
//	}
//	return curr;
//}
//
//void initialize(){
//	ifInit = 1;
//	memHead = sbrk(0);
//	memTail = memHead;
//	block * freeList = memHead;
//	freeList->size = 0;
//	freeList->available = 1;
//	freeList->next = NULL;
//	freeList->prev = NULL;
//}
void * bf_malloc(size_t size){
//	if(ifInit == 0){
//		initialize();
//		ifInit = 1;
//	}
  if(!ifInit){
    block * mcb = getNewMem(size+sizeof(block));
   	if(mcb == NULL){
			return NULL;
		}
    memHead = mcb;
    //memTail = memHead;
    ifInit = 1;
    //printf("return %p\n",mcb + sizeof(block));
    return (char*)mcb + sizeof(block);
  }
	if(size == 0){
		return NULL;
	}
	block * curr = memHead;
	block * best = NULL;
	while(curr != NULL){
		if(curr->size == size && curr->available){
			curr->available = 0;
			return (char *)curr + sizeof(block);
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
		//curr->next = mcb;
		//memTail = mcb;
		return (char*)mcb + sizeof(block);
	} else{
			best = split_blk(best,size);
			return (char*)best + sizeof(block);

	}
}
