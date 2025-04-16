CC=cc
test: test-file.o graph.o
	CC test-file.o graph.o -o test.out

main-run: main.o graph.o
	CC main.o graph.o -o main-run.out

main.o: main.c
	CC -c main.c

graph.o: graph.c
	CC -c graph.c

test-file.o: test-file.c
	CC -c test-file.c

clean:
	@rm -f *.out *.o
	@echo "Cleaned :D"