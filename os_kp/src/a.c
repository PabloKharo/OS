// Программа A принимает из стандартного потока ввода строки, а далее их отправляет программе С. Отправка строк должна производится построчно.
// Затем пересылает B количество отправленных символов



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "get_line/get_line.h"

int id1, id2;
int pipeAC[2];
int pipeAB[2];
int pipeCA[2];
int pipeCB[2];

void sig_handler(int signal) {
	kill(id1, SIGUSR1);
    kill(id2, SIGUSR1);

    close(pipeAC[0]);
    close(pipeAC[1]);
    close(pipeCA[0]);
    close(pipeCA[1]);
    close(pipeAB[0]);
    close(pipeAB[1]);
    close(pipeCB[0]);
    close(pipeCB[1]);
    exit(0);
}

int main(){

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
		printf("[%d] ", getpid());
        perror("Error signal ");
		return -1;
	}
	pipe(pipeAC);
	pipe(pipeAB);
	pipe(pipeCA);
    pipe(pipeCB);

    
	id1 = fork();
    if (id1 == -1)
	{
		perror("fork1");
		exit(-1);
	}
    else if(id1 == 0)
    {
        char name[] = "./b";
        char pAB[3] = "";
        sprintf(pAB, "%d", pipeAB[0]);
        char pCB[3] = "";
        sprintf(pCB, "%d", pipeCB[0]);

        close(pipeAC[0]);
        close(pipeAC[1]);
        close(pipeCA[0]);
        close(pipeCA[1]);
        close(pipeAB[1]);
        close(pipeCB[1]);

        execl(name, name, pAB, pCB, NULL);
    }
	else
	{		
		id2 = fork();
    	if (id2 == -1)
		{
		    perror("fork2");
		    exit(-1);
		}
		else if(id2 == 0)
		{
            char name[] = "./c";
            char pAC[3] = "";
            sprintf(pAC, "%d", pipeAC[0]);
            char pCA[3] = "";
            sprintf(pCA, "%d", pipeCA[1]);
            char pCB[3] = "";
            sprintf(pCB, "%d", pipeCB[1]);

            close(pipeAC[1]);
            close(pipeCA[0]);
            close(pipeAB[0]);
            close(pipeAB[1]);
            close(pipeCB[0]);
			execl(name, name, pAC, pCA, pCB, NULL);
		}
		else
		{
			char* line = NULL;
			int size;
			while((size = get_line(&line, STDIN_FILENO)) != 0)
			{
	            write(pipeAC[1], &size, sizeof(int));
	            write(pipeAC[1], line, size*sizeof(char));

                int ok;
                read(pipeCA[0], &ok, sizeof(int));

                write(pipeAB[1], &size, sizeof(int));
			}
			free(line);

            kill(id1, SIGUSR1);
            kill(id2, SIGUSR1);
		}	
	}


    close(pipeAC[0]);
    close(pipeAC[1]);
    close(pipeCA[0]);
    close(pipeCA[1]);
    close(pipeAB[0]);
    close(pipeAB[1]);
    close(pipeCB[0]);
    close(pipeCB[1]);

    
}