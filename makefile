all : build

build: list.o main.o pcb.o
	gcc list.o main.o pcb.o -o sim
	
main.o: main.c
	gcc -c main.c

pcb.o: pcb.h pcb.c
	gcc -c pcb.h pcb.c

valgrind: build
	valgrind --leak-check=full ./sim

clean:
	rm pcb.o main.o *.h.gch sim