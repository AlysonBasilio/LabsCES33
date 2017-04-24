#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#define TRUE 1

int n = 0; int pid;
char *argv[50];char temp[256];
int argc;
char hist[100][256];
int nhist = 0;


void type_prompt()
{	
   printf("[shell-PID=%i]$ ", getpid());
}

void read_command(char *argv[])
{	
   n = 0;
   gets(temp);
   if(temp[0]=='!'){
	if(nhist == 0)
		printf("Não há histórico a ser acessado.");
	else {
		if(temp[1]=='!'){
			strcpy(hist[nhist],hist[nhist-1]);
			nhist++;
		   	argv[n++] = strtok (hist[nhist-1]," ");
			while (argv[n-1] != NULL){
				argv[n++] = strtok (NULL, " ");
		   	}
		} else{
			argv[n] = strtok (temp,"!");
			int j = atoi(argv[n]);		
			if(j<nhist){
				argv[n++] = strtok(hist[j]," ");	
				strcpy(hist[nhist++],hist[j]);
				while (argv[n-1] != NULL){
					argv[n++] = strtok (NULL, "!");
			   	}
			}
			else
				strcpy(argv[n++],"Erro");
		}
	}
   }
   else{
	  strcpy(hist[nhist++],temp);
	  argv[n++] = strtok (temp," ");
	  while (argv[n-1] != NULL){
		argv[n++] = strtok (NULL, " ");
	  }
   }
}

int main()
{
   int status;
   argv[0] = NULL;
   while (TRUE) /* repeat forever */ {
      type_prompt(); /* display prompt on screen */
      read_command(argv); /* read input from terminal */
      if ((pid = fork()) != 0) /* fork off child process */ {
         printf("Esperando o filho: pid = %i\n", pid);
         waitpid(-1, &status, 0); /* wait for child to exit */
         printf("Waiting finished\n");
      } 
      else {
         if(strcmp(argv[0],"history")==0){
	   	printf("History\n");
		for(int i =0; i<nhist; i++){
			printf("%d %s \n",i, hist[i]);
		}
	 }
	 else if (strcmp(argv[0], "Erro") == 0)
		printf("Não executou. Erro: Comando inválido\n");
	 else if (execvp(argv[0], argv) == -1) /* execute command */
         	printf("Não executou. Erro: %s\n", strerror(errno));
      }
   }
   return 0;
}
