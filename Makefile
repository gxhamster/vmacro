CC=gcc
OUT=vmacro
CFLAGS=-Wall -Wextra -O3 -std=c11 -pedantic
CFLAGS_G=-Wall -Wextra -ggdb -p -std=c11 -pedantic
SRC=main.c macro.c actions.c actions-yank.c actions-delete.c

all: $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

debug: main.c macro.c actions.c
	$(CC) $(CFLAGS_G) -o $(OUT) $(SRC)

clean:
	rm $(OUT)
	

