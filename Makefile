Alanshell: main.o init.o prompt.o read_command.o run_exec.o avllib.o
	gcc main.o init.o prompt.o read_command.o run_exec.o avllib.o -o Alanshell

main.o: myhead.h
	gcc -c main.c

init.o :myhead.h
	gcc -c init.c 

prompt.o: myhead.h
	gcc -c prompt.c 

read_command.o: myhead.h
	gcc -c read_command.c 

run_exec.o: myhead.h
	gcc -c run_exec.c

avllib.o: myavl.h
	gcc -c avllib.c

clean:
	rm *.o Alanshell