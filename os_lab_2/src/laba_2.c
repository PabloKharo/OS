#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "child/child.h"
#include "get_line/get_line.h"


void ReadLineAndWriteToPipe(int fd, int pipe)
{
	char* line = NULL;
	int size;
	if((size = get_line(&line, fd)) == 0)
	{		
		exit(-1);
	}
	
	write(pipe, &size, sizeof(int));
	write(pipe, line, size*sizeof(char));

	free(line);
}

int main()
{
    int id1, id2;
	int pipe1[2];
	int pipe2[2];
	pipe(pipe1);
	pipe(pipe2);

	id1 = fork();
    if (id1 == -1)
	{
		perror("fork");
		exit(-1);
	}
    else if(id1 == 0)
    {
		close(pipe2[0]);
		close(pipe2[1]);

		Child(pipe1);
    }
	else
	{		
		id2 = fork();
    	if (id2 == -1)
		{
		    perror("fork");
		    exit(-1);
		}
		else if(id2 == 0)
		{
			close(pipe1[0]);
			close(pipe1[1]);

			Child(pipe2);
		}
		else
		{
			close(pipe1[0]);
			close(pipe2[0]);

			ReadLineAndWriteToPipe(STDIN_FILENO, pipe1[1]);
			ReadLineAndWriteToPipe(STDIN_FILENO, pipe2[1]);

			char* line = NULL;
			int size;
			while((size = get_line(&line, STDIN_FILENO)) != 0)
			{
				if(size > 10)
				{
					if(write(pipe2[1], &size, sizeof(int)) != sizeof(int))
						exit(-1);
					if(write(pipe2[1], line, size*sizeof(char)) != size*sizeof(char))
						exit(-1);
				}
				else
				{
					if(write(pipe1[1], &size, sizeof(int)) != sizeof(int))
						exit(-1);
					if(write(pipe1[1], line, size*sizeof(char)) != size*sizeof(char))
						exit(-1);
				}
			}
			free(line);

			close(pipe1[1]);
			close(pipe2[1]);
		}	
	}


	return 0;
}
