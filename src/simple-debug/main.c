#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/user.h>


void procmsg(const char* format, ...)
{
    va_list ap;
    fprintf(stdout, "[%d] ", getpid());
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}

void run_target(char* program) 
{
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) { 
        perror("ptrace PTRACE_TRACEME error\n");
        return;
    }
    execl(program, program, (char*)0);

}
void run_debugger(pid_t child_pid) 
{
    int wait_status, icounter = 0;

    procmsg("debugger started\n");

    do {
        wait(&wait_status);
        if (!WIFSTOPPED(wait_status))
            break;

        procmsg("wait returned signal: %s\n", strsignal(WSTOPSIG(wait_status)));

        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        unsigned instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);
        procmsg("RIP = 0x%08x. instr = 0x%08x\n", 
                regs.rip, instr);
        
        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
            perror("ptrace");
            return;
        }
        icounter++;
    }
    while (1);
    
    procmsg("executed %d instructions\n", icounter);
}

int main(int argc, char **argv) 
{
    pid_t child_pid;

    if (argc < 2) {
        fprintf(stderr, "Expected program name as argument\n");
        return -1;
    }
    
    child_pid = fork();
    if (child_pid == 0) { 
        run_target(argv[1]);
    } else { 
        run_debugger(child_pid);
    }
    return 0;
}
