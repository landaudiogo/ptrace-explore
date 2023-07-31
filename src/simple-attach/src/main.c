#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <string.h>
#include <sys/user.h>
#include <sys/wait.h>

void read_registers(pid_t tracee) {
    struct user_regs_struct regs;

    ptrace(PTRACE_GETREGS, tracee, NULL, &regs);
    unsigned instr = ptrace(PTRACE_PEEKTEXT, tracee, regs.rip, 0);
    printf("RIP = 0x%08llx. instr = 0x%08x\n", 
           regs.rip, instr);
}

void run_attach(pid_t tracee) 
{
    if (ptrace(PTRACE_ATTACH, tracee, NULL, NULL) < 0) {
        perror("Failed to attach to process\n");
        exit(1);
    }

    int icounter = 0;
    do {
        int wait_status; 

        waitpid(tracee, &wait_status, WUNTRACED);
        if (!WIFSTOPPED(wait_status))
            break;
        read_registers(tracee);

        if (ptrace(PTRACE_SINGLESTEP, tracee, NULL, NULL) < 0) {
            perror("Failed to single step");
            exit(1);
        }
        icounter++;
    } while(1);
    printf("[%d]: executed %d instructions\n", tracee, icounter);
    
}

int main(int argc, char** argv) 
{
    if (argc < 2) {
        printf("Missing argument target PID\n");
        exit(1);
    }

    pid_t tracee = atoi(argv[1]);
    run_attach(tracee);
}
