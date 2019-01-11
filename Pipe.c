#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Utilities.h"
#include "SubParser.h"
#include "StringParser.h"

#define READ 0
#define WRITE 1

#define BUFFSIZE 50000

extern char** environ;

int myPipe(char** subStrings, int subCount, int readFd, char* pwd, int i)
{
    int fd[2];
  	pid_t pid;

  	char** utilParser = parseString(subStrings[i]);

  	if (pipe(fd) == -1)
  	{
  	  	fprintf(stderr, "pipe error: %s\n", strerror(errno));
  	  	return -1;
  	}


  	if ((pid = fork()) == -1)
  	{
  	  	fprintf(stderr, "fork error: %s\n", strerror(errno));
  	  	return -1;
  	}
  	else if (pid == 0)  // Child
  	{
		int savedDup = dup(STDOUT_FILENO);
  		if (close(fd[READ]) == -1)
  		{
  		  	fprintf(stderr, "error closing the read side\n");
  		  	return -1;
		}

  		dup2(readFd, STDIN_FILENO);
  		dup2(fd[WRITE], STDOUT_FILENO);

  		if (!strcmp(utilParser[0], "dir"))
  		{
  		  	dir(pwd);
  		}
		else if (!strcmp(utilParser[0], "env"))
  		{
  		  	myEnviron(environ);
  		}
		else if (!strcmp(utilParser[0], "echo"))
  		{
  		  	echo(utilParser);
  		}
		else if (!strcmp(utilParser[0], "help"))
  		{
  		  	help();
  		}
  		else
  		{
			int size = arraySize(utilParser);
			if (size == 1)
			{			
				char* args[] = {utilParser[0], NULL};
				execvp(utilParser[0], args);
			}
  		  	else if (size >= 2)
  			{
				char* args[20];
				execArgs(utilParser, args, size);
				execvp(args[0], args);
  		  	}
		}

		exit(EXIT_SUCCESS);
  	}

  	else if (pid > 0)  // Parent
  	{
  		if (close(fd[WRITE]) == -1)
  		{
    		fprintf(stderr, "error closing the write side\n");
    		return -1;
    	}
  	}

  	if (i+1 == subCount)
  	{
		return fd[READ];
  	}
  
  	return myPipe(subStrings, subCount, fd[READ], pwd, i+1);
}

void evalPipe(char** subStrings, int count, char* pwd)
{
	if (subStrings[count - 1][0] == '>')
	{
		char** parseFileName = parseString(subStrings[count - 1]);

		// Remove Redirects Before Calling myPipe
		subStrings[count - 1] = NULL;
		count--;

		// Call myPipe
		int pipeOut = myPipe(subStrings, count, STDIN_FILENO, pwd, 0);
		
		// Create BUFF
		size_t returnedBuffSize;
		char BUFF[BUFFSIZE];
		
		if (parseFileName[0][1] == '>')
		{
			// Open File as Append
			int out = open(parseFileName[1], O_CREAT|O_WRONLY|O_APPEND, 0644);

			// Read from the Returned Descriptor from myPipe
			returnedBuffSize = read(pipeOut, BUFF, BUFFSIZE);

			// Write OutPut to File
			write(out, BUFF, returnedBuffSize);
		}
		else
		{
			// Open File as Truncate
			int out = open(parseFileName[1], O_CREAT|O_WRONLY|O_TRUNC, 0644);

			// Read from the Returned Descriptor from myPipe
			returnedBuffSize = read(pipeOut, BUFF, BUFFSIZE);

			// Write OutPut to File
			write(out, BUFF, returnedBuffSize);
		}
	}
	else
	{
		// Call myPipe
		int pipeOut = myPipe(subStrings, count, STDIN_FILENO, pwd, 0);
		// Create BUFF
		size_t returnedBuffSize;
		char BUFF[BUFFSIZE];

		// Read from the Returned Descriptor from myPipe
		returnedBuffSize = read(pipeOut, BUFF, BUFFSIZE);

		// Write OutPut to File
		write(STDOUT_FILENO, BUFF, returnedBuffSize);
	}
}