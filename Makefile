CFLAGS=-g -Wall -pipe -Werror -O3
LFLAGS=

default: simple_pid_test

test: simple_pid_test
	./simple_pid_test

simple_pid_test: simple_pid_test.o simple_pid.o
	$(CC) -o simple_pid_test simple_pid_test.o simple_pid.o $(LFLAGS)

%.o: %.c simple_pid.h
	$(CC) -c -o $@ $< $(CFLAGS)

format:
	clang-format -i *.c *.h

clean:
	rm -f simple_pid_test *.o
