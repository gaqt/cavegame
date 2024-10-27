LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRCS=src/cavegame.c src/player.c src/world.c
CFLAGS=-std=c23 -Wall -Wextra -Wpedantic
EXECUTABLE=cavegame

build:
	cc -o $(EXECUTABLE) $(SRCS) $(LIBS) $(CFLAGS) -Ofast -mtune=native

run: build
	./$(EXECUTABLE)

debug:
	cc -o $(EXECUTABLE) $(SRCS) $(LIBS) $(CFLAGS) -O0 -ftrapv -g3 -ggdb -D DEBUG
	./$(EXECUTABLE)
