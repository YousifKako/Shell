#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include "Pipe.h"
#include "SubParser.h"
#include "StringParser.h"

#define BUFFSIZE 256

char* cd(char** subStrings, char* pwd)
{
	if (!chdir(subStrings[1]))
	{
		if (!strcmp(subStrings[1], ".."))
		{
			int16_t lastBackSlash = 0;
			int16_t pwdLength = strlen(pwd);
			for (int16_t i = 0; pwd[i] != '\0'; ++i)
			{
				if (pwd[i] == '/')
				{
					lastBackSlash = i;
				}
			}
			pwd[lastBackSlash] = '\0';
		}
		else if (!strcmp(subStrings[1], "."))
		{
			// Do Nothing
			return pwd;
		}
		else
		{
			char newPwd[BUFFSIZE];
			strcpy(newPwd, pwd);
			strcat(newPwd, "/");
			strcat(newPwd, subStrings[1]);
			strcpy(pwd, newPwd);
		}
	}
	else
	{
		printf("Shell: cd: %s: No such file or directory\n", subStrings[1]);
	}

	return pwd;
}

void clear()
{
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void dir(const char* pwd)
{
	DIR* openDir = opendir(pwd);

	if (openDir)
	{
		struct dirent* readDir;
		while ((readDir = readdir(openDir)) != NULL)
		{
			write(STDOUT_FILENO, readDir->d_name, strlen(readDir->d_name));
      		write(STDOUT_FILENO, "\n", 1);
		}
	}
}

void help()
{
	char line[BUFFSIZE];
	FILE* fp = fopen("Help.txt", "r");

	while (fgets(line, BUFFSIZE, fp) != NULL)
	{
		write(STDOUT_FILENO, line, strlen(line));
	}

	write(STDOUT_FILENO, "\n", 1);
}

void myEnviron(char** env)
{
	for (int i = 0; env[i] != NULL; ++i)
	{
		write(STDOUT_FILENO, env[i], strlen(env[i]));
		write(STDOUT_FILENO, "\n", 1);
	}
}

void execArgs(char** array, char** args, int numOfArgs)
{
	if (array[0][0] == '|')
	{
		for (int i = 1; i != numOfArgs; ++i)
		{
			args[i - 1] = array[i];
		}
		args[numOfArgs-1] = NULL;
	}
	else
	{
		for (int i = 0; i != numOfArgs; ++i)
		{
			args[i] = array[i];
		}
		args[numOfArgs] = NULL;
	}
}

void rmNewLine(char* cmd)
{
	for (int i = 0; cmd[i] != '\0'; ++i)
	{
		if (cmd[i] == '\n')
		{
			cmd[i] = '\0';
			break;
		}
	}
}

int8_t echo(char** subStrings)
{
	char** subStr = parseString(subStrings[0]);

    if (subStr[1][0] == '$')
    {
        char* env;
        char* tmp = (char *)malloc(sizeof(char * ) * (strlen(subStr[1]) + 1));
        for (int i = 0; i < strlen(subStr[1]); ++i)
        {
            tmp[i] = subStr[1][i+1];
        }
        tmp[strlen(subStr[1] + 1)] = '\0';

        if ((env = getenv(tmp)) != NULL)
        {
            write(STDOUT_FILENO, env, strlen(env));
            write(STDOUT_FILENO, "\n", 1);
        }
        else
        {
            fprintf(stderr, "error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        free(tmp);
    }
    else
    {
        write(STDOUT_FILENO, subStr[1], strlen(subStr[1]));
        write(STDOUT_FILENO, "\n", 1);
    }
}

int8_t arraySize(char** array)
{
        int8_t size = 0;
        for (int i = 0; array[i] != NULL; ++i)
                size++;
        
        return size;
}