CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

all: bin/ls

bin/ls: src/ls-v1.0.0.c
	$(CC) $(CFLAGS) src/ls-v1.0.0.c -o bin/ls

bin/ls-v1.1.0: src/ls-v1.1.0.c
	$(CC) $(CFLAGS) src/ls-v1.1.0.c -o bin/ls-v1.1.0

clean:
	rm -f bin/ls bin/ls-v1.1.0 bin/ls-v1.2.0 bin/ls-v1.3.0 bin/ls-v1.4.0 bin/ls-v1.5.0 bin/ls-v1.6.0
bin/ls-v1.2.0: src/ls-v1.2.0.c
	$(CC) $(CFLAGS) src/ls-v1.2.0.c -o bin/ls-v1.2.0
bin/ls-v1.3.0: src/ls-v1.3.0.c
	$(CC) $(CFLAGS) src/ls-v1.3.0.c -o bin/ls-v1.3.0

bin/ls-v1.4.0: src/ls-v1.4.0.c
	$(CC) $(CFLAGS) src/ls-v1.4.0.c -o bin/ls-v1.4.0

bin/ls-v1.5.0: src/ls-v1.5.0.c
	$(CC) $(CFLAGS) src/ls-v1.5.0.c -o bin/ls-v1.5.0

bin/ls-v1.6.0: src/ls-v1.6.0.c
	$(CC) $(CFLAGS) src/ls-v1.6.0.c -o bin/ls-v1.6.0
