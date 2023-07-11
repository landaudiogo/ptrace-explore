# Compile

## Target Executables

These are the executables to be used to experiment with ptrace attach.

Compile the target executable: 
```bash
gcc -nostdlib -o target target.c
```

Compile the main executable: 
```bash
gcc -o main main.c
```

# Run

## Target Executables

Guarantee that the target executable is compiled in the first place. 

Then, we need to run the executable and get its PID. We do so by running the following:
```bash
./target &
echo $! | wl-copy
```

We then run the main executable with: 
```bash 
# sudo ./main <Ctrl-V>
# e.g. 
sudo ./main 22917
```
