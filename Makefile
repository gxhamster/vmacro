CC=gcc
OUT=macro
CFLAGS=-Wall -Wextra -O3
CFLAGS_G=-Wall -Wextra -ggdb

all: main.c macro.c actions.c
	$(CC) $(CFLAGS) -o $(OUT) actions.c macro.c main.c

debug: main.c macro.c actions.c
	$(CC) $(CFLAGS_G) -o $(OUT) actions.c macro.c main.c

clean:
	rm $(OUT)
	

