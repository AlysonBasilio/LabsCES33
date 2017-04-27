#include <windows.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define N 10
#define debugtxt(FORMAT) printf(" TID %d: " #FORMAT "\n",GetCurrentThreadId())
#define debug(FORMAT, ARGS...) printf("TID %d: " #FORMAT "\n",GetCurrentThreadId(),ARGS)
int line_counter;

// Funcoes e variaveis do buffer
int start;
int end;
int buffer[N];
int count;
int last_produced_item;
int last_consumed_item;

void insert_item(int item) {
	debug("Inserting item %d",item);
	buffer[end]=item;
	end=(end+1)%N;
	debugtxt("Item inserted!");
}
int remove_item() {
	debugtxt("Removing item ...");
	int item = buffer[start];
	start=(start+1)%N;
	debug("Item %d removed!",item);
	return item;
}

// Funcoes e variaveis das threads
HANDLE handleThread[2];
DWORD threadId[2];
const int producer = 0;
const int consumer = 1;

void sleep() {
	debugtxt("Sleeping ...");
	SuspendThread(GetCurrentThread());
	debugtxt("Waked up!");
}
void wakeup(const int x) {
	debug("Waking up %d ...",threadId[x]);
	ResumeThread(handleThread[x]);
	debug("Waking up signal sent to %d!",threadId[x]);
}

// Produtor e consumidor ...
int produce_item() {
	debugtxt("Producing item ...");
	last_produced_item++;
	debug("Produced item %d",last_produced_item);
	return last_produced_item;
}
void consume_item(int item) {
	debugtxt("Consuming item ...");
	last_consumed_item = item;
	debug("Consumed item %d",item);
}

DWORD WINAPI producerFunc( LPVOID lpParam ) {
	debugtxt("Starting producer");
	int item;
	while(TRUE) {
		item=produce_item();
		if(count == N) sleep(); //Se buffer está cheio, produtor dorme
		insert_item(item);
		count = count + 1;
		if(count == 1) wakeup(consumer);//Se há algo para ser retirado do buffer, acorda consumidor
	}
	debugtxt("Ending producer");
	return 0;
}

DWORD WINAPI consumerFunc( LPVOID lpParam ) {
	debugtxt("Starting consumer");
	int item;
	while(TRUE) {
		if(count == 0) { //Se não há nada pra ser removido do buffer, consumidor dorme.
		   // Sleep(3000); //Descomentar:forca disputa
			sleep();
		}
		item = remove_item();
		count = count -1;
		if(count == N-1) wakeup(producer); // Se há espaço no buffer, acorda o produtor.
		consume_item(item);
	}
	debugtxt("Ending consumer");
	return 0;
}

int main() {
   last_produced_item = 0;
   start = 0;
   end = 0;
   LPTHREAD_START_ROUTINE threadFunc[2] = { producerFunc, consumerFunc };
   int i;

   for(i=0;i<2;i++) {
		handleThread[i] = CreateThread(
            NULL,               // default security attributes
            0,                  // use default stack size
            threadFunc[i],      // thread function pointer
            &i,     			// argument to thread function
            0,                  // use default creation flags
            &threadId[i]);   // returns the thread identifier
   }
   WaitForMultipleObjects(2, handleThread, TRUE, INFINITE);

   for(i=0;i<2;i++) {CloseHandle(handleThread[i]);
  }

}
