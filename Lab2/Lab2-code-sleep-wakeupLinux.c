#include <stdio.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define N 10

int line_counter;

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
pthread_mutex_t the_mutex;
const int producer = 0;
const int consumer = 1;

// Produtor e consumidor ...
int produce_item() {
	printf(" TID %lu: Producing item ...\n",pthread_self());
	last_produced_item++;
	printf("TID %lu: Produced item %d\n",pthread_self(),last_produced_item);
	return last_produced_item;
}

void consume_item(int item) {
	printf(" TID %lu: Consuming item ...\n",pthread_self());
	last_consumed_item = item;
	printf("TID %lu: Consumed item %d\n",pthread_self(),item);
}

void *producerFunc( void *lpParam ) {
	threadId[producer] = pthread_self();
	printf(" TID %lu: Starting producer\n",pthread_self());
	int item;
	while(TRUE) {
		item=produce_item();
		if(count == N){
			printf(" TID %lu: Sleeping ...\n",pthread_self());
			pthread_cond_wait(&condp, &the_mutex); //Faz o produtor dormir esperando sinal do consumidor para acordar
			printf(" TID %lu: Waked up!\n",pthread_self());
		}
		insert_item(item);
		count = count + 1;
		if(count == 1){
			printf("TID %lu: Waking up %lu ...\n",pthread_self(),threadId[consumer]);
			pthread_cond_signal(&condc);//Envia o sinal para o consumidor acordar
			printf("TID %lu: Waking up signal sent to %lu!\n",pthread_self(),threadId[consumer]);
		}
	}
	printf(" TID %lu: Ending producer\n",pthread_self());
	return 0;
}

void *consumerFunc( void *lpParam ) {
	threadId[consumer] = pthread_self();
	printf(" TID %lu: Starting consumer\n",pthread_self());
	int item;
	long int i;
	while(TRUE) {
		if(count == 0) {
		   	for(i=0; i<150000;i++);// Sleep(3000); //Descomentar:forca disputa
			printf(" TID %lu: Sleeping ...\n",pthread_self());
			pthread_cond_wait(&condc, &the_mutex);//Faz o consumidor dormir esperando o sinal do produtor para acordar
			printf(" TID %lu: Waked up!\n",pthread_self());
		}
		item = remove_item();
		count = count -1;
		if(count == N-1){
			printf("TID %lu: Waking up %lu ...\n",pthread_self(),threadId[producer]);
			pthread_cond_signal(&condp);//Envia sinal para o produtor acordar;
			printf("TID %lu: Waking up signal sent to %lu!\n",pthread_self(),threadId[producer]);
		}
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
	pthread_mutex_init(&the_mutex,0);//É necessária para a utilização das funções de sleep e wakeup
	pthread_cond_init(&condc,0);//Variáveis que servem como um sensor, ao receberem sinais,
	pthread_cond_init(&condp,0);//acordam threads.
	pthread_create(&pro, 0, producerFunc, 0);
	pthread_create(&con, 0, consumerFunc, 0);
	pthread_join(con,0);
	pthread_join(pro,0);
	pthread_cond_destroy(&condc);
	pthread_cond_destroy(&condp);
	pthread_mutex_destroy(&the_mutex);
}
