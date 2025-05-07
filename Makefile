CC=cc
CFLAGS = -Wall -Wextra -g -fsanitize=undefined,address
test: test-file.o graph.o input.o
	$(CC) $(CFLAGS) test-file.o graph.o input.o -o test.out
main-run: main.o graph.o input.o
	$(CC) $(CFLAGS) main.o graph.o input.o -o main-run.out

csrrg: csrrg-test.o graph.o input.o
	$(CC) $(CFLAGS) csrrg-test.o graph.o input.o -o csrrg.out
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

graph.o: graph.c
	$(CC) $(CFLAGS) -c graph.c

test-file.o: test-file.c
	$(CC) $(CFLAGS) -c test-file.c

csrrg-test.o: csrrg-test.c
	$(CC) $(CFLAGS) -c csrrg-test.c
input.o: input.c
	$(CC) $(CFLAGS) -c input.c

clean:
	@rm -f *.out *.o
	@echo "Cleaned :D"