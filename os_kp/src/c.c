// Программа C печатает в стандартый вывод, полученную строку от программы A. После получения программа C отправляет программе А сообщение о том, что строка получена.

// До тех пор пока программа А не примет «сообщение о получение строки» от программы С, она не может отправялять следующую строку программе С

// Затем пересылает B количество отправленных символов

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int pipeAC;
int pipeCA;
int pipeCB;


void sig_handler(int signal) {
	close(pipeAC);
	close(pipeCA);
	close(pipeCB);

    exit(0);

}

int main(int argc, char *argv[]){
    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
        perror("[C] Error signal ");
		return -1;
	}
    pipeAC = atoi(argv[1]);
    pipeCA = atoi(argv[2]);
    pipeCB = atoi(argv[3]);

    int sizeA;
    while(read(pipeAC, &sizeA, sizeof(int))> 0){
        char line[sizeA+1];
        line[sizeA] = '\0';
	    read(pipeAC, line, sizeA * sizeof(char));
        printf("[C] Get from A: %s\n", line);

        int ok = 1;
        write(pipeCA, &ok, sizeof(int));

        write(pipeCB, &sizeA, sizeof(int));

    }

}
