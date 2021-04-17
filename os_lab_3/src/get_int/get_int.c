#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "get_int.h"

int get_char(int fd)
{
    char c;
    if (read(fd, &c, 1) == 1)
        return (unsigned char)c;
    return EOF;
}

int get_int(int fd, int* res)
{
	int c;
	int sign = 1;
	int i = 0;
	*res = 0;
	while((c = get_char(fd)) != EOF && c != ' ' && c != '\n' && c!= '\r')
	{
		if(i == 0 && c == '-'){
			sign = -1;
		}
		else if(c >= '0' && c <= '9'){
			*res *= 10;
			*res += (int)(c - '0');
		}else{
			return -1;
		}

		i++;
	}
	*res *= sign;

	return 0;
}

