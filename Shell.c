#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Pipe.h"
#include "SubParser.h"
#include "Redirect.h"
#include "Utilities.h"
#include "StringParser.h"

#define BUFFSIZE 50000
#define CWDBUFF 256

int main(int argc, char** argv, char** environ)
{
	char command[CWDBUFF];
	int count = 0;
	char* BUFF;
	char** subStrings;
	char** utilParser;

	char* pwd = getcwd(BUFF, CWDBUFF);

	while (1)
	{
		if (feof(stdin))
		{
			break;
		}
		printf("%s>", pwd);
		fgets(command, CWDBUFF, stdin);
		rmNewLine(command);
		subStrings = parser(command, &count);
		utilParser = parseString(subStrings[0]);

		if (!strcmp(utilParser[0], "cd"))
		{
			if (utilParser[1] == NULL)
			{
				continue;
			}
			cd(utilParser, pwd);
		}
		else if (!strcmp(utilParser[0], "clr") || !strcmp(utilParser[0], "clear"))
		{
			clear();
		}
		else if (!strcmp(utilParser[0], "quit") || !strcmp(utilParser[0], "exit"))
		{
			exit(EXIT_SUCCESS);
		}
		else
		{
			pid_t pid;
			int status;

			if ((pid = fork()) == -1)
			{
				fprintf(stderr, "file error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			}
			else if (pid == 0)	// Child
			{
				if (!strcmp(subStrings[count - 1], "&"))
				{
					subStrings[count - 1] = NULL;
					count--;
				}
				if (!strcmp(utilParser[0], "dir"))
				{
					int savedDup;

					if (subStrings[1] == NULL)
					{
						dir(pwd);
					}
					else if (subStrings[1][0] == '>')
					{
						if (subStrings[1][1] == '>')
						{
							outRedirect(subStrings[1], O_APPEND);
							dir(pwd);
						}
						else
						{
							outRedirect(subStrings[1], O_TRUNC);
							dir(pwd);
						}
					}
					else if (subStrings[1][0] == '|')
					{
						evalPipe(subStrings, count, pwd);
					}

					exit(EXIT_SUCCESS);
				}
				else if (!strcmp(utilParser[0], "env"))
				{
					int savedDup;

					if (subStrings[1] == NULL)
					{
						myEnviron(environ);
					}
					else if (subStrings[1][0] == '>')
					{
						if (subStrings[1][1] == '>')
						{
							outRedirect(subStrings[1], O_APPEND);
							myEnviron(environ);
						}
						else 
						{
							outRedirect(subStrings[1], O_TRUNC);
							myEnviron(environ);
						}
					}
					else if (subStrings[1][0] == '|')
					{
						evalPipe(subStrings, count, pwd);
					}

					exit(EXIT_SUCCESS);
				}
				else if (!strcmp(utilParser[0], "echo"))
				{
					int savedDup;

					if (subStrings[1] == NULL)
					{
						echo(subStrings);
					}
					else if (subStrings[1][0] == '>')
					{
						if (subStrings[1][1] == '>')
						{
							outRedirect(subStrings[1], O_APPEND);
							echo(subStrings);
						}
						else
						{
							outRedirect(subStrings[1], O_TRUNC);
							echo(subStrings);
						}
					}
					else if (subStrings[1][0] == '|')
					{
						evalPipe(subStrings, count, pwd);
					}
					
					exit(EXIT_SUCCESS);
				}
				else if (!strcmp(utilParser[0], "help"))
				{
					int savedDup;

					if (subStrings[1] == NULL)
					{
						help();
					}
					else if (subStrings[1][0] == '>')
					{
						if (subStrings[1][1] == '>')
						{
							outRedirect(subStrings[1], O_APPEND);
							help();
						}
						else
						{
							outRedirect(subStrings[1], O_TRUNC);
							help();
						}
					}
					else if (subStrings[1][0] == '|')
					{
						evalPipe(subStrings, count, pwd);
					}

					exit(EXIT_SUCCESS);
				}
				// External Functions
				else
				{
					int8_t utilParserSize = arraySize(utilParser);
					
					if (utilParserSize >= 1)
					{
						char** parseStr = parseString(subStrings[0]);
						char * myArgs[20];
						
						if (subStrings[1] == NULL)
						{
							execArgs(parseStr, myArgs, utilParserSize);
							execvp(myArgs[0], myArgs);
						}
						else if (subStrings[1][0] == '|')
						{
							evalPipe(subStrings, count, pwd);
						}
						else if (subStrings[1][0] == '<')
						{
							if (subStrings[2] == NULL)
							{
								inRedirect(subStrings);
								execArgs(parseStr, myArgs, utilParserSize);
								execvp(myArgs[0], myArgs);								
							}
							else if (subStrings[2][0] == '>')
							{
								if (subStrings[2][1] == '>')
								{
									inRedirect(subStrings);
									outRedirect(subStrings[2], O_APPEND);
									execArgs(parseStr, myArgs, utilParserSize);
									execvp(myArgs[0], myArgs);
								}
								else
								{
									inRedirect(subStrings);
									outRedirect(subStrings[2], O_TRUNC);
									execArgs(parseStr, myArgs, utilParserSize);
									execvp(myArgs[0], myArgs);
								}
							}
							else if (subStrings[2][0] == '|')
							{
								char pBUFF[BUFFSIZE];
								inRedirect(subStrings);
								int p = myPipe(subStrings, count, STDIN_FILENO, pwd, 2);
								int r = read(p, pBUFF, BUFFSIZE);

								if (subStrings[count - 1][0] == '>')
								{
									if (subStrings[count - 1][1] == '>')
									{
										outRedirect(subStrings[count - 1], O_APPEND);
									}
									else
									{
										outRedirect(subStrings[count - 1], O_TRUNC);
									}
								}
								
								write(STDOUT_FILENO, pBUFF, r);
							}
						}
						else if (subStrings[1][0] == '>')
						{
							if (subStrings[1][1] == '>')
							{
								outRedirect(subStrings[1], O_APPEND);
								execArgs(parseStr, myArgs, utilParserSize);
								execvp(myArgs[0], myArgs);
							}
							else
							{
								outRedirect(subStrings[1], O_TRUNC);
								execArgs(parseStr, myArgs, utilParserSize);
								execvp(myArgs[0], myArgs);
							}
						}
					}
					exit(EXIT_SUCCESS);
				}
				exit(EXIT_SUCCESS);
			}
			else if (pid > 0)	// Parent
			{
				if (!strcmp(subStrings[count - 1], "&"))
				{
					printf("PID:[%d]\n", pid);
				}
				else
				{
					waitpid(pid, &status, 0);
				}
			}
		}
	}
	
	exit(EXIT_SUCCESS);
	return 0;
}
