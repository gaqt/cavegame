LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRCS=src/cavegame.c src/player.c src/world.c
CFLAGS=-Wall -Wextra -Wpedantic
EXECUTABLE=cavegame

build:
	cc -o $(EXECUTABLE) $(SRCS) $(LIBS) $(CFLAGS) -O3

run: build
	./$(EXECUTABLE)

debug:
	cc -o $(EXECUTABLE) $(SRCS) $(LIBS) $(CFLAGS) -O0 -ftrapv -D DEBUG
	./$(EXECUTABLE)
