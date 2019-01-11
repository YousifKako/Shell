CFLAGS = -g

objects = Shell.c Pipe.c Redirect.c Utilities.c StringParser.c SubParser.c

Event : $(objects)
	cc -o Shell $(objects)

.PHONY : clean
clean :
	rm Shell $(objects)
