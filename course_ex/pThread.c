#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void * p(void* params){
	printf("Hello world from thread <%d>",params);
}

int main(void){
	int num;
	scanf("%d",&num);
	int nThread = num;
	pthread_t * threads = malloc (nThread*sizeof(*threads));

	for(int i = 0; i<nThread;i++){
		pthread_create(&threads[i],NULL,p,&i);
	}

	for(int i = 0; i<nThread;i++){
		pthread_join(threads[i],NULL);
	}
	free(threads);

}