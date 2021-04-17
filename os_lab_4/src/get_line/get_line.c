#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "get_line.h"

int get_char(int fd)
{
    char c;
    if (read(fd, &c, 1) == 1)
        return (unsigned char)c;
    return EOF;
}

int get_line(char** line_, int fd)
{
	free(*line_);
	int c;
	int size = 0;
	int cap = 4;
	char* line = (char*)malloc(cap * sizeof(char));
	while((c = get_char(fd)) != EOF && c != '\n' && c!= '\r')
	{
		if(size == cap)
		{
			cap *= 2;
			line = (char*)realloc(line, cap * sizeof(char));
			if(line == NULL)
				exit(-1);
		}
		line[size] = c;
		++size;
	}
	if(size == cap)
	{
		cap += 1;
		line = (char*)realloc(line, cap * sizeof(char));
		if(line == NULL)
			exit(-1);
	}
	line[size] = '\0';
	*line_ = line;

	return size;
}


