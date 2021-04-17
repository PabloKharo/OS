#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>

#include <semaphore.h>

#include "get_line/get_line.h"

#define SEM_FAILED ((sem_t *) 0)


const int BUFF = 128;
sem_t* child_sem[2];
char* chsem_names[2] = { "/child0", "/child1" };
sem_t* par_sem[2];
char* parsem_names[2] = { "/par0", "/par1" };
char* mmap_files[2];
int id[2];


void OpenSems() {
    for (int i = 0; i < 2; ++i) {
        child_sem[i] = sem_open(chsem_names[i], O_CREAT, 0666, 0);
        if (child_sem[i] == SEM_FAILED) {
            perror("Error open: ");
            exit(-1);
        }
    }
	for (int i = 0; i < 2; ++i) {
        par_sem[i] = sem_open(parsem_names[i], O_CREAT, 0666, 1);
        if (par_sem[i] == SEM_FAILED) {
            perror("Error open: ");
            exit(-1);
        }
    }
}

void CloseSems() {
    for (int i = 0; i < 2; ++i) {
        if (sem_close(child_sem[i])) {
            perror("Error close: ");
        }
    }
	for (int i = 0; i < 2; ++i) {
        if (sem_close(par_sem[i])) {
            perror("Error close: ");
        }
    }
}

void UnlinkSems() {
    for (int i = 0; i < 2; ++i) {
        if (sem_unlink(chsem_names[i]) < 0) {
            perror("Error unlink: ");
        }
        if (sem_unlink(parsem_names[i]) < 0) {
            perror("Error unlink: ");
        }
    }
}

void Munmap(){
    munmap(mmap_files[0], BUFF);
    munmap(mmap_files[1], BUFF);
}

void ReverseLine(char* line, int size)
{
    for(int i = 0; i < size / 2; ++i)
    {
        char tmp = line[size - i - 1];
        line[size - i - 1] = line[i];
        line[i] = tmp;      
    }
}

void sig_handlerchd(int sig){
    CloseSems();
    exit(0);
}

int Child(int id, char* file_path){
    if(signal(SIGTERM, sig_handlerchd) == SIG_ERR){
        perror("Error signal: ");
        exit(-1);
    }
    int fd;
    if ((fd = open(file_path, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
    	perror(file_path);
        exit(-1);
    }

    int size = 0;
    while(1) {
        sem_wait(child_sem[id]);

        memcpy(&size, mmap_files[id], sizeof(size));
        if (size == 0) {
            sem_post(par_sem[id]);
            break;
        }
	    char line[size+2];
        memcpy(line, mmap_files[id] + sizeof(size), size*sizeof(char));
        line[size] = '\n';
        line[size+1] = '\0';

        ReverseLine(line, size);

        write(fd, line, size+1);

        sem_post(par_sem[id]);
    }

	close(fd);

	return 0;
}

void WriteToMmap(int id, int size, char* line)
{
	sem_wait(par_sem[id]);
	memcpy(mmap_files[id], (char*)(&size), sizeof(size));
    memcpy(mmap_files[id] + sizeof(size), line, size*sizeof(char));
	sem_post(child_sem[id]);
}

void Parent()
{
	char* line = NULL;
    int size;

    while ((size = get_line(&line, STDIN_FILENO)) != 0) {
        if(size > 10){
			WriteToMmap(1, size, line);
		}else{
			WriteToMmap(0, size, line);
		}

    }
    free(line);
}

void Close(){
    Munmap();
    UnlinkSems();
    CloseSems();
}

void sig_handler(int sig){
    kill(id[0], SIGTERM);
    kill(id[1], SIGTERM);
    Close();
    exit(0);
}

int main()
{        
    char* file_path = NULL;
    if(get_line(&file_path, STDIN_FILENO) == 0){
        free(file_path);
        exit(0);
    }
    char* file_path2 = NULL;
    if(get_line(&file_path2, STDIN_FILENO) == 0){
        free(file_path);
        free(file_path2);
        exit(0);
    }

    mmap_files[0] = mmap(NULL, BUFF, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mmap_files[1] = mmap(NULL, BUFF, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (mmap_files[0] == MAP_FAILED) {
        perror("Error mmap: ");
        exit(-1);
    }
    if (mmap_files[1] == MAP_FAILED) {
        perror("Error mmap: ");
        munmap(mmap_files[0], BUFF);
        exit(-1);
    }
    OpenSems();

    if(signal(SIGINT, sig_handler) == SIG_ERR){
        perror("Error signal: ");
        exit(-1);
    }


    id[0] = fork();
    if (id[0] == -1)
	{
		perror("Error fork: ");
        Close();   
		exit(-1);
	}
    else if(id[0] == 0)
    {
		Child(0, file_path);
    }
	else
	{		
		id[1] = fork();
    	if (id[1] == -1)
		{
		    perror("Error fork: ");
            Close();
		    exit(-1);
		}
		else if(id[1] == 0)
		{
		    Child(1, file_path2);
		}
		else
		{
            Parent();
            UnlinkSems();
            kill(id[0], SIGTERM);
            kill(id[1], SIGTERM);       
		}	
	}
    free(file_path);
    free(file_path2);

    Munmap();
    CloseSems();
	return 0;
}
