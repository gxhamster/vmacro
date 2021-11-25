CC=gcc
OUT=macro
CFLAGS=-g -Wall -Wextra 

all: main.c macro.c
	$(CC) $(CFLAGS) -o $(OUT) macro.c main.c

clean:
	rm $(OUT)
	

