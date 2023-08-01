#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <string.h>
#include <sys/user.h>
#include <sys/wait.h>

struct user_regs_struct 
read_regs(pid_t tracee) {
    struct user_regs_struct regs;
    memset(&regs, 0, sizeof(struct user_regs_struct));

    ptrace(PTRACE_GETREGS, tracee, NULL, &regs);

    return regs;
}

void
print_reverse_word(unsigned long word) {
    for (int i=0; i<8; i++) {
        printf("%02x ", (unsigned char) (word >> i*8) & (0xff));
    }
    printf("\n");
}

void 
read_data(pid_t tracee, unsigned long address, size_t size) { 
    unsigned long data;
    printf("data at address: 0x%08lx\n", address);
    for (int i=0; i < size; i+=8) {
        data = ptrace(PTRACE_PEEKDATA, tracee, address, NULL);
        print_reverse_word(data);
    }
}

void 
set_regs(pid_t tracee, struct user_regs_struct *regs) {
    ptrace(PTRACE_SETREGS, tracee, NULL, regs);
}

void run_attach(pid_t tracee, unsigned long map_address) 
{
    struct user_regs_struct old_regs;
    struct user_regs_struct new_regs; 
    memset(&new_regs, 0, sizeof(struct user_regs_struct));

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

        if (icounter == 0) {
            old_regs = read_regs(tracee);
            printf("0x%llx\n", old_regs.rip);
            memcpy(&new_regs, &old_regs, sizeof(struct user_regs_struct));
            // new_regs.rip = map_address + 0x72;
            new_regs.rip = old_regs.rip; // - 0x5e;
            read_data(tracee, new_regs.rip, 0x5);
            set_regs(tracee, &new_regs);
            getchar();
        }

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
    pid_t tracee; 
    unsigned long map_address;

    if (argc < 3) {
        printf("Missing argument target PID\n");
        exit(1);
    }

    tracee = atoi(argv[1]);
    map_address = (unsigned long) strtol(argv[2], NULL, 16);

    run_attach(tracee, map_address);
}
