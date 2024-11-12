.PHONY: all

FFLAGS= -ansi -std=c99 -Wall -Wextra -Wswitch-enum -Wdouble-promotion -Wconversion -Wno-sign-conversion -fsanitize=undefined -g3 -fno-omit-frame-pointer

LFLAGS=-lraylib -lm /usr/lib/libraylib.so
all: main.x86_64

main.x86_64: main.o
	gcc $(FFLAGS) -o $@ $^ $(LFLAGS)

%.o: src/%.c
	gcc $(FFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o main.x86_64

.PHONY: run
run:
	make clean && make main.x86_64 && ./main.x86_64 
