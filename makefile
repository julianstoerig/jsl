.PHONY: all

FFLAGS= -ansi -std=c99 -Wall -Wextra -Wswitch-enum -Wdouble-promotion -Wconversion -Wno-sign-conversion -fsanitize=undefined -g3 -fno-omit-frame-pointer

LFLAGS=-lraylib -lm /usr/lib/libraylib.so
all: main

main: main.o
	gcc $(FFLAGS) -o $@ $^ $(LFLAGS)

%.o: src/%.c
	gcc $(FFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o main

.PHONY: run
run:
	make clean && make main && ./main
