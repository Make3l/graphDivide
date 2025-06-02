CC=cc
#CFLAGS = -Wall -Wextra -g -fsanitize=undefined,address
test: test-file.o graph.o input.o tests.o
	$(CC) test-file.o graph.o input.o tests.o -o test.out
main-run: main.o graph.o input.o
	$(CC) main.o graph.o input.o -o main-run.out

mac: interface.o graph.o input.o tests.o
	$(CC) -Wall -fPIC -O2 -dynamiclib interface.o graph.o input.o tests.o -o ../lib/libgraphlib.dylib

linux: interface.o graph.o input.o tests.o
	$(CC) -Wall -fPIC -O2 -shared interface.o graph.o input.o tests.o -o ../lib/libgraphlib.so

windows: interface.o graph.o input.o tests.o
	$(CC) -Wall -fPIC -O2 -shared interface.o graph.o input.o tests.o -o ../lib/libgraphlib.dll

interface.o: interface.c
	$(CC) -Wall -fPIC -O2 -c interface.c

main.o: main.c
	$(CC) -c main.c

graph.o: graph.c
	$(CC) -c graph.c

test-file.o: test-file.c
	$(CC) -c test-file.c
input.o: input.c
	$(CC) -c input.c
tests.o: tests.c
	$(CC) -c tests.c

clean:
	@rm -f *.out *.o
	@rm -f ../lib/libgraphlib.*
	@echo "Cleaned :D"