all: Task_7

Task_7: main.o lib.a
	gcc main.o lib.a -o Task_7
main.o: main.c
	gcc -c main.c -o main.o
lib.a: parse.o table.o
	ar rcs lib.a parse.o table.o
parse.o: ../parse.c
	gcc -c ../parse.c -o parse.o
table.o: ../table.c
	gcc -c ../table.c -o table.o

clean:
	rm *.o Task_7 lib.a