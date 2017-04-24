#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#define TRUE 1

int NCOM = 0; int n = 0; int pid;
char *argv[];char temp[256];
int argc;
char history[1000]="";

void type_prompt()
{	
   printf("[shell-PID=%i]$ ", getpid());
}

void read_command(char *argv[])
{	
   n = 0;
   gets(temp);
   argv[n++] = strtok (temp," ");
   while (argv[n-1] != NULL)
   argv[n++] = strtok (NULL, " ");
}

void save_history()
{	
   strcat(history,NCOM);
   strcat(history," ");
   strcat(history,temp);
   strcat(history,"\n");
   printf("%s",history);
}

int main()
{
   int status;
   argv[0] = NULL;
   while (TRUE) /* repeat forever */ {
      type_prompt(); /* display prompt on screen */
      read_command(argv); /* read input from terminal */
      save_history(argv);
      if ((pid = fork()) != 0) /* fork off child process */ {
         printf("Esperando o filho: pid = %i\n", pid);
         waitpid(-1, &status, 0); /* wait for child to exit */
         printf("Waiting finished");
      } 
      else {
         if (execvp(argv[0], argv) == -1) /* execute command */
            printf("Não executou. Erro: %s\n", strerror(errno));
      }
   }
   return 0;
}
