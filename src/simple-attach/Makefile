CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c,$(OBJ)/%.o, $(SRCS))

BINDIR=bin

all: $(BINDIR)/main $(BINDIR)/target

$(BINDIR)/main: $(SRC)/main.c
	$(CC) $(CFLAGS) -o $@ $(SRC)/main.c 

$(BINDIR)/target: $(SRC)/target.c
	$(CC) $(CFLAGS) -nostdlib -o $@ $(SRC)/target.c

clean: 
	$(RM) -r $(BINDIR)/*
