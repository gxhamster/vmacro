CC=gcc
OUT=macro
CFLAGS=-g -Wall -Wextra 

all: main.c
	$(CC) $(CFLAGS) -o $(OUT) main.c

clean:
	rm $(OUT)
	

