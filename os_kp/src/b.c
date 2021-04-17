// Программа B пишет в стандартный вывод количество отправленных символов программой А и количество принятых символов программой С.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int pipeAB;
int pipeCB;


void sig_handler(int signal) {
    close(pipeAB);
    close(pipeCB);
    exit(0);
}

int main(int argc, char *argv[]){
    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
		printf("[%d] ", getpid());
        perror("Error signal ");
		return -1;
	}
    pipeAB = atoi(argv[1]);
    pipeCB = atoi(argv[2]);

    int sizeA;
    int sizeB;
    while(read(pipeAB, &sizeA, sizeof(int)) > 0 && read(pipeCB, &sizeB, sizeof(int)) > 0){
        printf("[B] Get A: %d; Get C: %d\n", sizeA, sizeB);
    }

}