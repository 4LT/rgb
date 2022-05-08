CC=cc
BASEFLAGS=-std=c99 -Wall -pedantic $(MORE_ARGS)

ifeq ($(shell uname), Darwin)
	LDFLAGS= -framework SDL2
else
	LDFLAGS= -lSDL2 -lm
endif

ifdef OPMZ
	CFLAGS=$(BASEFLAGS) -O2 -march=native
else
	CFLAGS=$(BASEFLAGS) -ggdb
endif

.PHONY: all

all: rgb kd kd2 interleave

main.o: main.c  canvas.h
	$(CC) $(CFLAGS) -c main.c

kd.o: kd.c canvas.h
	$(CC) $(CFLAGS) -c kd.c

kd2.o: kd2.c canvas.h
	$(CC) $(CFLAGS) -c kd2.c

interleave.o: interleave.c canvas.h
	$(CC) $(CFLAGS) -c interleave.c

canvas.o: canvas.c canvas.h
	$(CC) $(CFLAGS) -c canvas.c

rgb: canvas.o main.o Makefile
	$(CC) $(CFLAGS) -o rgb main.o canvas.o $(LDFLAGS)

kd: canvas.o kd.o Makefile
	$(CC) $(CFLAGS) -o kd kd.o canvas.o $(LDFLAGS)

kd2: canvas.o kd2.o Makefile
	$(CC) $(CFLAGS) -o kd2 kd2.o canvas.o $(LDFLAGS)

interleave: canvas.o interleave.o Makefile
	$(CC) $(CFLAGS) -o interleave interleave.o canvas.o $(LDFLAGS)
