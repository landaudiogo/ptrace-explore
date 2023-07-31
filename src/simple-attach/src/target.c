#include <time.h>

int primitive_strlen(char* string) { 
    int i; 
    for(i=0; string[i] != '\0'; i++);
    return i-2;
}

void primitive_print(char* msg) 
{
    int len = primitive_strlen(msg);
    // syscall write to stdout
    asm(
        "mov    $1, %%rdi;"
        "mov    %[msg], %%rsi;"
        "mov    $15, %%rdx;"
        "mov    $1, %%rax;"
        "syscall;"
        :
        : [msg] "m" (msg), 
          [len] "m" (len)
    );
}

void primitive_exit(int exit_status) {
    asm(
        // Exit with status `0`
        "mov %[exit_status], %%rdi;"
        "mov $60, %%rax;"
        "syscall;" 
        :
        : [exit_status] "m" (exit_status)
    );
}


void _start() { 
    int src = 0; 
    char* msg_before = "before sleep\n\0";
    char* msg_after = "after sleep\n\0";

    primitive_print(msg_before);

    struct timespec time;
    time.tv_sec = 600;
    time.tv_nsec = 0;
    struct timespec* reqtime = &time;
    struct timespec* remtime;
    asm(
        // Sleep for duration `reqtime`
        "mov %[reqtime], %%rdi;"
        "mov %[remtime], %%rsi;"
        "mov $35, %%rax;"
        "syscall;" 
        :
        : [reqtime] "m" (reqtime),
          [remtime] "m" (remtime)
    );
    // primitive_print(msg_after);
    primitive_exit(0);
}
