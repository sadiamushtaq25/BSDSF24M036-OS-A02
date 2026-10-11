CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGETS = bin/ls \
          bin/ls-v1.1.0 \
          bin/ls-v1.2.0 \
          bin/ls-v1.3.0 \
          bin/ls-v1.4.0 \
          bin/ls-v1.5.0 \
          bin/ls-v1.6.0

.PHONY: all clean

all: $(TARGETS)

bin:
	mkdir -p bin

bin/ls: src/ls-v1.0.0.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin/ls-v1.1.0: src/ls-v1.1.0.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin/ls-v1.2.0: src/ls-v1.2.0.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin/ls-v1.3.0: src/ls-v1.3.0.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin/ls-v1.4.0: src/ls-v1.4.0.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin/ls-v1.5.0: src/ls-v1.5.0.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin/ls-v1.6.0: src/ls-v1.6.0.c | bin
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
