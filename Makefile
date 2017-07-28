Alanshell: main.o init.o prompt.o read_command.o
	gcc main.o init.o prompt.o read_command.o -o Alanshell

main.o: myhead.h
	gcc -c main.c

init.o :myhead.h
	gcc -c init.c 

prompt.o: myhead.h
	gcc -c prompt.c 

read_command.o: myhead.h
	gcc -c read_command.c 

clean:
	rm main.o init.o prompt.o read_command.o