## Report # Assignment #1: Malloc Library Part 1

##### Yijia Li (yl730)

### 1. Overview

For this assignment, I implement my own version of memory allocation function, malloc and free. And I use two memory allocation policy first fit and best fit to implement. So there are four main function`ff_malloc()`, `bf_malloc()`, `ff_free()`, `bf_free()`. In this report, I will introduce the implement structure and details of the four function, and then analyze performance results.



### 2. Design, implementation and test 

The first key point is to construct an appropriate data structure to store meta information, so that we can get the corresponding information conveniently. My meta information block is defined as follows.


```c
typedef struct block_t{
  size_t size;
  int available;
  struct block_t * next;
  struct block_t * prev;
} block;
```

This structure has four variables. First, "size" records how many bytes in the allocated memory. Second, "available" represents if the block can be used to store data. The two block_t* next and previous point to the previous and next block adjacent to current block respectively. 

I use a double linked list of block to manage all available blocks, which is called freeList, and use a pointer of block points to the head of the freeList. When calling memory allocation function malloc, first, I check if there is block available in the freeList. If not, the function will call `sbrk()` to get more money and create a new block to store its information. If the freeList isn't empty, the function will iterate freeList from its head. There are three possibilities may occur. First, there exists one block which has exactly the same size as we need. This block will be removed from freeList, update its previous and next block, and change its available to 0. Then address of this block will be returned. Second, if the size of a block is larger than required size + sizeof(block), we will split this block by calling `split_blk()` to split this block into two blocks, the first one has the size required, the second one will be put into freeList. The first block will be returned. The third case is that there's no eligible block, so the function will call `sbrk()` and get a new memory.

As for `free()` function, what we need to do is to free a block, put it back to freeList and then merge it. First, I iterate the freeList from head to find an appropriate position to insert freed block, and then check if it can merge with its previous and next blocks. If so, the two or three blocks will be merged into one block, and corresponding information will be updated simultaneously.

For best fit strategy, the most significant difference between FF and BF is that BF always choose the smallest suitable block. Therefore, in `bf_malloc()`, I add a new variable "best", which is a block * pointing to the current eligible smallest block. After traversing, if best isn't NULL, the best block will be splitted.


### 3. Performance Results & Analysis

My performance results are as follow,

| Pattern | First-Fit<Br>Exec Time | First-Fit<Br>Fragmentation | Best-Fit<Br>Exec Time | Best-Fit<Br>Fragmentation |
| ------- | :--------------------: | :------------------------: | :-------------------: | :-----------------------: |
| Equal   |         17.26s         |          0.999889          |        17.38s         |         0.999889          |
| Small   |         13.16s         |          0.501176          |         4.22s         |         0.854333          |
| Large   |         41.43s         |          0.859026          |        52.742s        |         0.976159          |


For `equal_size_allocs`, it uses the same umber of bytes (128) in all of its malloc calls. Therefore, for best fit and first fit,  they should have relatively similar performance. Their fragmentation are the same and the difference of execution time is very small. The execution time for best fit is a little larger than one for first fit is due to its best selection process.

For `small_range_rand_allocs`, it works with allocations of random size, ranging from 128 - 512 bytes. The program first malloc is a large number of these random regions. Then the program alternates freeing a random selection of 50 of these allocated regions, and mallocing 50 more regions with a random size from 128 - 512 bytes. Since best fit always chooses the smallest suitable block, there will be more small block left. It fragmentation should be larger than first fit. My execution outcome proves this point. In this test, best fit has a better execution time than first fit. In my mind, the reason may be bytes of allocated memory is relatively small. So it will be easier to find the same size block through best fit, while first fit need always split blocks, which takes much longer time than finding the smallest suitable block.

For `large_range_rand_allocs`, it works with allocations of random size, ranging from
32 - 64K bytes (in 32B increments). The program first malloc is a large number of these random regions. Then the program alternates freeing a random selection of 50 of these allocated regions, and mallocing 50 more regions with a random size from 32 - 64K bytes. As we can see, fragmentation of best fit is still larger than the one of first fit due to the same reason as small test. And the execution time of best fit turns to be larger than first fit, which is opposite to outcome of small test. It's hard for best fit to find a block with exactly the same size. So its time for searching an splitting will become longer. 

There is also some points that can be optimized in this project. For example, single linked list may be a more simplified structure than doubly linked list. And the available signal can also be simplified.