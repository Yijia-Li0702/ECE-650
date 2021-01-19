#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif


int main(int argc, char *argv[])
{
  const unsigned NUM_ITEMS = 10;
  int i;
  int size;
  int sum = 0;
  int expected_sum = 0;
  int *array[NUM_ITEMS];

  size = 4;
  expected_sum += size * size;//16
  printf("array[0],size=16+32=48\n");
  array[0] = (int *)MALLOC(size * sizeof(int));//16+32=48
  for (i=0; i < size; i++) {
    array[0][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[0][i];
  } //for i

  size = 16;
  expected_sum += size * size;
  printf("array[1],size=64+32=96\n");
  array[1] = (int *)MALLOC(size * sizeof(int));//64+32=96
  for (i=0; i < size; i++) {
    array[1][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[1][i];
  } //for i

  size = 8;
  expected_sum += size * size;
  printf("array[2],size=32+32=64\n");
  array[2] = (int *)MALLOC(size * sizeof(int));//32+32=64
  for (i=0; i < size; i++) {
    array[2][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[2][i];
  } //for i

  size = 32;
  expected_sum += size * size;
  printf("array[3],size=128+32=160\n");
  array[3] = (int *)MALLOC(size * sizeof(int));//128+32=160
  for (i=0; i < size; i++) {
    array[3][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[3][i];
  } //for i
  
  printf("free array[0]\n");
  FREE(array[0]);
   print();
  printf("free array[2]\n");
  FREE(array[2]);

  size = 7;
  expected_sum += size * size;
  printf("array[4],size=28+32=60\n");
  array[4] = (int *)MALLOC(size * sizeof(int));//28+32=60
  for (i=0; i < size; i++) {
    array[4][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[4][i];
  } //for i

  size = 256;
  expected_sum += size * size;
  printf("array[5],size=1056\n");
  array[5] = (int *)MALLOC(size * sizeof(int));//1056
  for (i=0; i < size; i++) {
    array[5][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[5][i];
  } //for i
  
  printf("free array[5]\n");
  FREE(array[5]);
  printf("free array[1]\n");
  FREE(array[1]);
  printf("free array[3]\n");
  FREE(array[3]);
  
  print();

  size = 23;
  expected_sum += size * size;
  printf("array[6],size=92+32=124\n");
  array[6] = (int *)MALLOC(size * sizeof(int));//92+32=124
  for (i=0; i < size; i++) {
    array[6][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[6][i];
  } //for i
  
   print();

  size = 4;
  expected_sum += size * size;
  printf("array[7],size=16+32=48\n");
  array[7] = (int *)MALLOC(size * sizeof(int));//16+32=48
  for (i=0; i < size; i++) {
    array[7][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[7][i];
  } //for i
  printf("free array[4]\n");
  FREE(array[4]);

  size = 10;
  expected_sum += size * size;
  printf("array[8],size=40+32=72\n");
  array[8] = (int *)MALLOC(size * sizeof(int));//40+32=72
  for (i=0; i < size; i++) {
    array[8][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[8][i];
  } //for i

  size = 32;
  expected_sum += size * size;
  printf("array[9],size=128+32=160\n");
  array[9] = (int *)MALLOC(size * sizeof(int));//128+32=160
  for (i=0; i < size; i++) {
    array[9][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[9][i];
  } //for i
  printf("free array[6]\n");
  FREE(array[6]);
  printf("free array[7]\n");
  FREE(array[7]);
  printf("free array[8]\n");
  FREE(array[8]);
  printf("free array[9]\n");
  FREE(array[9]);

  if (sum == expected_sum) {
    printf("Calculated expected value of %d\n", sum);
    printf("Test passed\n");
  } else {
    printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
    printf("Test failed\n");
  } //else

  return 0;
}
