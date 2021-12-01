CC=gcc
OUT=vmacro
CFLAGS=-Wall -Wextra -O3 -std=c11 -pedantic
CFLAGS_G=-Wall -Wextra -ggdb -p -std=c11 -pedantic

all: main.c macro.c actions.c
	$(CC) $(CFLAGS) -o $(OUT) actions.c macro.c main.c

debug: main.c macro.c actions.c
	$(CC) $(CFLAGS_G) -o $(OUT) actions.c macro.c main.c

clean:
	rm $(OUT)
	

