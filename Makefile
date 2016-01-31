all: par-shell fibonacci par-shell-terminal

par-shell: par-shell.o commandlinereader.o list.o list_terminal.o 
	gcc -o par-shell -pthread par-shell.o commandlinereader.o list.o list_terminal.o 
par-shell.o: par-shell.c commandlinereader.h list.h
	gcc -Wall -pthread -g -c par-shell.c	

par-shell-terminal: par-shell-terminal.o commandlinereader.o
	gcc -o par-shell-terminal par-shell-terminal.o commandlinereader.o

par-shell-terminal.o: par-shell-terminal.c commandlinereader.h
	gcc -g -Wall -c par-shell-terminal.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -pthread -Wall -g -c commandlinereader.c

list.o: list.c list.h
	gcc -pthread -Wall -g -c list.c
	
list_terminal.o : list_terminal.c list_terminal.h
	gcc -pthread -Wall -g -c list_terminal.c


fibonacci:  fibonacci.c
	gcc -o fibonacci -g -Wall fibonacci.c


clean:
	rm -f *.o par-shell fibonacci
