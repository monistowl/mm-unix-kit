CC ?= cc
CFLAGS ?= -O2 -std=c99 -Wall -Wextra

all: bin/roll

bin/roll: src/roll.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin:
	mkdir -p bin
