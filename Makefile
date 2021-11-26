CC=gcc
OUT=macro
CFLAGS=-Wall -Wextra -O3
CFLAGS_G=-Wall -Wextra -ggdb

all: main.c macro.c
	$(CC) $(CFLAGS) -o $(OUT) macro.c main.c

debug: main.c macro.c
	$(CC) $(CFLAGS_G) -o $(OUT) macro.c main.c

clean:
	rm $(OUT)
	

