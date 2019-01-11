#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char* duplicate(const char* left, const char* right)
{
        unsigned size = right - left + 1;
        char* ret = (char*)malloc(sizeof(char) * size);
        memcpy(ret, left, size);
        ret[size - 1] = '\0';

        return ret;
}

char** parseString(const char* command)
{
        if (command[0] == ' ')
        {
                return NULL;
        }

        int16_t numOfSpaces = 0;

        for (short i = 0; command[i] != '\0'; i++)
        {
                if (command[i] == ' ')
                {
                        numOfSpaces++;
                }
        }

        char** returnString = (char **)malloc(sizeof(char *) * (numOfSpaces + 2));
        short j = 0;

        const char* left = command;
        const char* right = command;
        while (*right)
        {
        	if (*right == ' ')
                {
                        returnString[j++] = duplicate(left, right);

                        left = right + 1;
                        while (*left && *left == ' ')
                        {
                                ++left;
                        }

                        right = left;
                        if (*right)
                        {
                                ++right;
                        }
                }
                else
                {
                        ++right;
                }
        }

        if (right > left)
        {
                returnString[j++] = duplicate(left, right);
        }

        returnString[j] = NULL;

        return returnString;
}