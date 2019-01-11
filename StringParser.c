#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

char** parser(const char* string, int* const count)
{
	int numOfOperators = 0;
	for (int i = 0; string[i] != '\0'; i++)
	{
		switch (string[i])
		{
		case '>':
			if (string[i + 1] == '>')
				++i;
		case '|':
		case '<':
		case '&':
			numOfOperators++;
		}
	}

	*count = numOfOperators + 1;
	char** subStrings = (char **)malloc(sizeof(char *) * (*count));
	int lastSubString = 0;

	int start = 0;
	int strLen = strlen(string) + 1;
	for (int i = 0; i < strLen; i++)
	{
		switch (string[i])
		{
			case '>':
			case '|':
			case '<':
			case '&':
			case '\0':
			{
				int j = i - 1;
				while (j >= start && string[j] == ' ')
					j--;

				size_t len = j - start + 1;
				subStrings[lastSubString] = (char *)malloc(sizeof(char) * (len + 1));
				memcpy(subStrings[lastSubString], &string[start], len * sizeof(char));
				subStrings[lastSubString][len] = '\0';
				++lastSubString;

				start = i;
				if (string[start] != '\0' && string[i + 1] == '>')
					i++;
			}
		}
	}

	return subStrings;
}