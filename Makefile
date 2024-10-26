LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRCS=src/main.c src/button.c
FLAGS=-Wall -Wextra -Wpedantic

build:
	cc -o logalc $(SRCS) $(LIBS) $(FLAGS) -O3

run: build
	./logalc

debug:
	cc -o logalc $(SRCS) $(LIBS) $(FLAGS) -O0 -ftrapv
	./logalc
