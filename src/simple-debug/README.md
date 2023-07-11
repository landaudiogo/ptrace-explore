# Compile

## Simple Debugger

To compile the main debugger file `main.c`:
```bash
gcc -o main main.c
```

## Targets 

To compile the `C` sample target file `target.c`:
```bash
gcc -o target target.c
```

To compile the Assembly sample target file `target-asm.asm`:
```bash
nasm -f elf64 target-asm.asm
ld -o target-asm target-asm.o
```

# Run

First compile the sample targets and the main executable.

Then run the main executable providing a single argument that points to the
executable file of the `target-file`:
```bash
# ./main <target-file>
# e.g. 
./main target-asm
```
