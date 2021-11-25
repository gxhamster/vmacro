CC=gcc
OUT=macro
CFLAGS=-g -Wall -Wextra -O3

all: main.c macro.c
	$(CC) $(CFLAGS) -o $(OUT) macro.c main.c

clean:
	rm $(OUT)
	

