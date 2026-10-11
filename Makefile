CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

all: bin/ls

bin/ls: src/ls-v1.0.0.c
	$(CC) $(CFLAGS) src/ls-v1.0.0.c -o bin/ls

bin/ls-v1.1.0: src/ls-v1.1.0.c
	$(CC) $(CFLAGS) src/ls-v1.1.0.c -o bin/ls-v1.1.0

clean:
	rm -f bin/ls bin/ls-v1.1.0
