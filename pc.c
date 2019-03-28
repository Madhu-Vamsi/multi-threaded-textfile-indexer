//producer consumer problem
//author: Madhu Vamsi
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>

# define MAX (3)
# define MAX_FILE_PATH_LENGTH (500)

sem_t empty;
sem_t full;
sem_t mutex;

int loops;
char tmp[MAX_FILE_PATH_LENGTH];
// int buffer[MAX];
char buffer[MAX][MAX_FILE_PATH_LENGTH];
int fill=0;
int use=0;

void put(char* i){
	strcpy(buffer[fill],i);
	fill=(fill+1)%MAX;
}

char* get(){

	// int len = strlen(buffer[use]);
	
	strcpy(tmp,buffer[use]);
	use=(use+1)%MAX;
	return tmp;
}

void* producer(void* arg){
	int i;
	char snum[5];
	for(i=0;i<=loops;i++){
		sem_wait(&empty);
		sem_wait(&mutex);
		// itoa(i, snum, 10);
		sprintf(snum,"%d",i);
		put(snum);
		printf("putted %s\n",snum );
		sem_post(&mutex);
		sem_post(&full);
	}
}

void* consumer(void* arg){
	int i;
	for(i=0;i<=loops/2;i++){
		sem_wait(&full);
		sem_wait(&mutex);
		char* tmp=get();
		sem_post(&mutex);
		sem_post(&empty);
		printf("consumed %s\n", tmp );
	}
}

int main(){
	loops = 41;
	sem_init(&mutex, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, MAX);
	int rc;
	pthread_t p;
	pthread_t c1;
	pthread_t c2;
	pthread_t c3;
	rc = pthread_create(&p, NULL, producer, NULL);
	assert(rc==0);
	rc = pthread_create(&c1, NULL, consumer, NULL);
	assert(rc==0);
	rc = pthread_create(&c2, NULL, consumer, NULL);
	assert(rc==0);
	// rc = pthread_create(&c3, NULL, consumer, NULL);
	// assert(rc==0);
	pthread_join(p,NULL);
	pthread_join(c1,NULL);
	pthread_join(c2,NULL);
	// pthread_join(c3,NULL);
	printf("done main!!\n" );
}
