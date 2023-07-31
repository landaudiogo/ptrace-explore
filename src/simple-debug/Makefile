CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c,$(OBJ)/%.o, $(SRCS))
$(info [${OBJS}])

BINDIR=bin

all: $(BINDIR)/main $(BINDIR)/target $(BINDIR)/target-asm

$(BINDIR)/main: $(SRC)/main.c
	$(CC) $(CFLAGS) -o $@ $<

$(BINDIR)/target: $(SRC)/target.c
	$(CC) $(CFLAGS) -o $@ $<

$(BINDIR)/target-asm: $(OBJ)/target-asm.o
	ld -o $@ $<

$(OBJ)/target-asm.o: $(SRC)/target-asm.asm
	nasm -f elf64 $< -o $@

clean: 
	$(RM) -r $(BINDIR)/* $(OBJ)/*
