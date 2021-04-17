#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "../get_line/get_line.h"
#include "child.h"

void ReverseLine(char* line, int size)
{
    for(int i = 0; i < size / 2; ++i)
    {
        char tmp = line[size - i - 1];
        line[size - i - 1] = line[i];
        line[i] = tmp;      
    }
}

void Child(int pipe[2])
{
    close(pipe[1]);
    int size;
	read(pipe[0], &size, sizeof(int));

	char file_path[size+1];
    file_path[size] = '\0';
	read(pipe[0], file_path, size * sizeof(char));

    int fd;
	if ((fd = open(file_path, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
    {
		perror(file_path);
        exit(-1);	
	}

    while(read(pipe[0], &size, sizeof(int)) > 0)
    {
        char line[size+2];
        line[size] = '\n';
        line[size+1] = '\0';
	    read(pipe[0], line, size * sizeof(char));

        ReverseLine(line, size);
        
        write(fd, line, (size+1) * sizeof(char));
    }
    
    close(pipe[0]);
    close(fd);
}
