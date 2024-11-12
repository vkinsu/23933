all: Task_4

Task_4: main.o list.o
	gcc main.o list.o -o Task_4

main.o: main.c
	gcc -c main.c -o main.o
list.o: list.c
	gcc -c list.c -o list.o

clean:
	rm Task_4 main.o list.o