/*
  Following:
  https://blog.nelhage.com/2010/08/write-yourself-an-strace-in-70-lines-of-code/
 */

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <assert.h>

int wait_for_syscall(pid_t child) {
    int status;
    while(1) {
        ptrace(PTRACE_SYSCALL, child, 0, 0);
        waitpid(child, &status, 0);
        if(WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
            return 0;
        if(WIFEXITED(status))
            return 1;
    }
}

int main(int argc, char **argv) {
    assert(argc >= 2);

    pid_t child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME);
        kill(getpid(), SIGSTOP);
        
        char *args[argc];
        memcpy(args, argv+1, (argc-1) * sizeof(char*));
        args[argc-1] = NULL;
        return execvp(args[0], args);
    }
    else {
        int status, syscall, retval;
        waitpid(child, &status, 0); // 0: no options
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
        while (1) {
            if (wait_for_syscall(child) != 0) break;
            syscall = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX);
            fprintf(stderr, "syscall(%d) = ", syscall);

            if (wait_for_syscall(child) != 0) break;
            retval = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*RAX);
            fprintf(stderr, "%d\n", retval);
        }
    }
    return 0;
}

/*
  1. Print out arguments to syscalls: do PTRACE_GETUSER to read words
  of memory sequentially from tracee's address-space

  2. Attach to a running process like strace -p

  3. Understand why we need ORIG_RAX and RAX on 64-bit machines. Ans:
  it's an offset into user_regs_struct. syscall number goes in RAX,
  return value goe in RAX.
 */
