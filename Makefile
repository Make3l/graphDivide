CC=cc
#CFLAGS = -Wall -Wextra -g -fsanitize=undefined,address
test: test-file.o graph.o input.o
	$(CC) test-file.o graph.o input.o -o test.out
main-run: main.o graph.o input.o
	$(CC) main.o graph.o input.o -o main-run.out

csrrg: csrrg-test.o graph.o input.o
	$(CC) csrrg-test.o graph.o input.o -o csrrg.out
main.o: main.c
	$(CC) -c main.c

graph.o: graph.c
	$(CC) -c graph.c

test-file.o: test-file.c
	$(CC) -c test-file.c

csrrg-test.o: csrrg-test.c
	$(CC) -c csrrg-test.c
input.o: input.c
	$(CC) -c input.c

clean:
	@rm -f *.out *.o
	@echo "Cleaned :D"