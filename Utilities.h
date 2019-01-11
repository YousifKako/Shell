#ifndef Utilities_H
#define Utilities_H

char* cd(char** subStrings, char* pwd);
void clear();
void dir(const char* pwd);
void help();
void myEnviron(char** env);
void execArgs(char** array, char** args, int numOfArgs);
void rmNewLine(char* cmd);
int8_t echo(char** subStrings);
int8_t arraySize(char** array);

#endif // Utilities_H
