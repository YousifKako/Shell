#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "Utilities.h"
#include "Pipe.h"
#include "StringParser.h"
#include "SubParser.h"

int outRedirect(char* file, int flag)
{
    char** subStr = parseString(file);
    int out = open(subStr[1], O_CREAT|O_WRONLY|flag, 0644);
    int savedDup;

    if (out == -1)
    {
        close(out);
        fprintf(stderr, "file error: %s\n", strerror(errno));
		return EXIT_FAILURE;
    }
    else
    {
		savedDup  = dup(STDOUT_FILENO);
        dup2(out, STDOUT_FILENO);
    }

    close(out);
    return savedDup;
}

int inRedirect(char** subString)
{
    char** subStr = parseString(subString[1]);
    int in = open(subStr[1], O_RDONLY);
    int savedDup;

    if (in == -1)
    {
        close(in);
        fprintf(stderr, "file error: %s\n", strerror(errno));
		return EXIT_FAILURE;
    }
    else
    {
        savedDup  = dup(STDIN_FILENO);
        dup2(in, STDIN_FILENO);
    }

    close(in);
    return in;
}