String Parser(s):

	StringParser.c
		char** parser(const char* string, int* const count)
			This parser will parse the initial string (command)
			e.g. command --> ps -l | grep Shell | wc >> out.txt
			     Parsed  --> {"ps -l", "| grep Shell", "| wc", ">> out.txt"}

			     I do this because index [1] will always be either '|', '<', '>', '>>'
			     therefore it is easier to look for what comes first/next

		char** parseString(char* command)
			This parser will tokenize a string (command)
			e.g. command --> "ps -l"
			     Parsed  --> {"ps", "-l"}
			     More...
			     command --> "| grep Shell"
			     Parsed  --> {"|", "grep", "Shell"}

Utilities:

	Utilities.c
		char* cd(char** subStrings, char* pwd)  <-- NOT included in the forked portion of the program
			- This function will utilize the system call chdir() to check the current
			  process's CWD

		void clear()  <-- NOT included in the forked portion of the program
			- This function is print 60 newlines ("\n")
			
		void dir(const char* pwd)  <-- IS included in the forked portion of the program... You may Pipe '|' OR Redirect Out '>'/'>>'
			- This function will write everything in the pwd to STDOUT_FILENO

		void help()  <-- IS included in the forked portion of the program... You may Pipe '|' OR Redirect Out '>'/'>>'
			- This function reads from a file named Help.txt and will write the
			  content to STDOUT_FILENO

		void myEnviron(char** env)  <-- IS included in the forked portion of the program... You may Pipe '|' OR Redirect Out '>'/'>>'
			- This function will write all of the Environment Varibles to STDOUT_FILENO

		int8_t echo(char** subStrings)  <-- IS included in the forked portion of the program... You may Pipe '|' OR Redirect Out '>'/'>>'
			- This function writes everything after the keyword echo... You may also echo Environment Varibles
			  using the dollar sign '$' to STDOUT_FILENO

		int8_t arraySize(char** array)
			- This function returns the size of an array. This is utilized in certain portions of the program

		void execArgs(char** array, char** args, int numOfArgs)
			- This function is to tokenize the flags for exec'ed programs... This function does not return, it
			  puts the flags in the "args" array that is passed in

		void rmNewLine(char* cmd)
			- This function removed the newline at the end of the command... when I read in the input from the prompt
			  it adds a newline, so this function removes that.

Pipe:
	Pipe.c
		void evalPipe(char** subStrings, int count, char* pwd)
			- This is the function that is called from main, and it will call myPipe(). It will check for redirection
			  in/out and will write to STDOUT_FILENO

		int myPipe(char** subStrings, int subCount, int readFd, char* pwd, int i)
			- This is the Pipe function, it returns the last fd[read] so I can read and then write the content of that
			  file descriptor to STDOUT_FILENO  <-- This is all does in the evalPipe() function and evalPipe() is called from main
			- This function is writen recursively... It takes in the whole command string, a count/size, the file descriptor but
			  initially it takes in the STDIN_FILENO and then it changes according to the recursion. This also takes in the pwd and
			  'i' to keep track of where we are in the recursion process

Redirection:
	Redirect.c
		int outRedirect(char* file, int flag)
			- This function saves STDOUT_FILENO then dups the STDOUT_FILENO to the specified file, it also return the saved dup

		int inRedirect(char** subString)
			- This function saves STDIN_FILENO then dups the STDIN_FILENO to the specified program, it also return the saved dup






















