#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define N 2

// Funcoes e variaveis do buffer
int start;
int end;
int buffer[N];
int count;
int last_produced_item;
int last_consumed_item;

void insert_item(int item) {
	printf("TID %lu: Inserting item %d\n",pthread_self(),item);
	buffer[end]=item;
	end=(end+1)%N;
	printf(" TID %lu: Item inserted!\n",pthread_self());
}
int remove_item() {
	printf(" TID %lu: Removing item ...\n",pthread_self());
	int item = buffer[start];
	start=(start+1)%N;
	printf("TID %lu: Item %d removed!\n",pthread_self(),item);
	return item;
}

// Funcoes e variaveis das threads
long unsigned int threadId[2];
pthread_cond_t condc, condp;
sem_t full, empty, mutex;
const int producer = 0;
const int consumer = 1;

void up(sem_t *sem, const char * name) {
	/*if(strcmp("full",name)==0 && end==start){
		printf("TID %lu: Up %s ...\n",pthread_self(),name);
		sem_wait(sem);	
		printf("TID %lu: Up %s complete!\n",pthread_self(),name);
	}*/
	printf("TID %lu: Up %s ...\n",pthread_self(),name);
	sem_post(sem);	
	printf("TID %lu: Up %s complete!\n",pthread_self(),name);
}
void down(sem_t *sem, const char * name) {
	printf("TID %lu: Down %s ...\n",pthread_self(),name);
	sem_wait(sem);	
	printf("TID %lu: Down %s complete!\n",pthread_self(),name);
/*	if(strcmp("empty",name)==0 && end==start){
		printf("TID %lu: Down %s ...\n",pthread_self(),name);
		sem_wait(sem);	
		printf("TID %lu: Down %s complete!\n",pthread_self(),name);
	}
	else if(strcmp("full",name)==0 && end==start){
		printf("TID %lu: Down %s ...\n",pthread_self(),name);
		sem_post(sem);	
		printf("TID %lu: Down %s complete!\n",pthread_self(),name);
	}*/
}

// Produtor e consumidor ...
int produce_item() {
	printf("TID %lu: Producing item ...\n",pthread_self());
	last_produced_item++;
	printf("TID %lu: Produced item %d\n",pthread_self(),last_produced_item);
	return last_produced_item;
}
void consume_item(int item) {
	printf("TID %lu: Consuming item ...\n",pthread_self());
	last_consumed_item = item;
	printf("TID %lu: Consumed item %d\n",pthread_self(),item);
}

void *producerFunc( void *lpParam ) {
	threadId[producer] = pthread_self();	
	printf(" TID %lu: Starting producer\n",pthread_self());
	int item;
	while(TRUE) {
		item=produce_item();
		down(&empty,"empty");
		sem_wait(&mutex);
		insert_item(item);
		sem_post(&mutex);
		up(&full,"full");
	}
	printf(" TID %lu: Ending producer\n",pthread_self());
	return 0;
}

void *consumerFunc( void *lpParam ) {
	threadId[consumer] = pthread_self();
	printf(" TID %lu: Starting consumer\n",pthread_self());
	int item;
	while(TRUE) {
		down(&full,"full");
		sem_wait(&mutex);
		item = remove_item();
		sem_post(&mutex);
		up(&empty,"empty");
		consume_item(item);
	}
	printf(" TID %lu: Ending consumer\n",pthread_self());
	return 0;
}

int main() {
	last_produced_item = 0;
	start = 0;
	end = 0;
	pthread_t pro,con;
	sem_init(&full,0,0);
	sem_init(&empty,0,N-1);
	sem_init(&mutex,0,1);
	pthread_create(&pro, 0, producerFunc, 0);
	pthread_create(&con, 0, consumerFunc, 0);
	pthread_join(con,0);
	pthread_join(pro,0);
}

