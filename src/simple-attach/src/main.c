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
        data = ptrace(PTRACE_PEEKDATA, tracee, address + i, NULL);
        print_reverse_word(data);
    }
}

unsigned long long 
read_word(pid_t tracee, unsigned long address, size_t size) {
    return ptrace(PTRACE_PEEKDATA, tracee, address, NULL);
}

void 
write_data(pid_t tracee, unsigned long address, unsigned long long *data, size_t len) {
    for(int i=0; i < len; i++)
        ptrace(PTRACE_POKETEXT, tracee, address+i*8, data[i]);
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
            printf("rip = 0x%llx\n", old_regs.rip);
            memcpy(&new_regs, &old_regs, sizeof(struct user_regs_struct));
            // new_regs.rip = map_address + 0x72;
            // new_regs.rip = old_regs.rip; // - 0x63; // - 0x5e;
            set_regs(tracee, &new_regs);

            unsigned long long rbp_0x10 = read_word(tracee, new_regs.rbp - 0x10, 8);
            printf("rbp_0x10 = 0x%llx\n", rbp_0x10);

            unsigned char data[] = { 
                //word1 
                0x48, 0x31, 0xff,
                0xb8, 0x3c, 0x00, 0x00, 0x00,
                // word2 
                0x0f, 0x05, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            };
            unsigned char data2[] = { 
                0xbf, 0x01, 0x00, 0x00, 0x00,               // mov edi, 0x1
                0x48, 0x8b, 0x75, /* WORD 1*/ 0xf0,                     // movabs rsi, 0x402000; check _start
                0xba, 0x0e, 0x00, 0x00, 0x00,               // mov edx, 0xe
                0xb8, 0x01, /* WORD 2 */ 0x00, 0x00, 0x00,               // mov eax, 0x1
                0x0f, 0x05,                                 // syscall
                0xcc, 
                0x90, 0x90, /* WORD 3 */
                // word1 
                0x48, 0x31, 0xff,
                0xb8, 0x3c, 0x00, 0x00, 0x00,
                // word2 
                0x0f, 0x05, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            };
            write_data(tracee, new_regs.rip, (unsigned long long*)data2, 5);
            read_data(tracee, new_regs.rip, 24);
            getchar();
        }

        if (ptrace(PTRACE_CONT, tracee, NULL, NULL) < 0) {
            perror("Failed to single step");
            exit(1);
        }
        icounter++;
        printf("Trapped %d\n", icounter);
        getchar();
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
