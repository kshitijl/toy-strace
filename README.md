# Toy straces

These are two toy programs inspired by `strace`. 

The first and simpler one simply uses `LD_PRELOAD` to override C
library and system calls made by a program.

The second uses the `ptrace` system call actually used by `strace`.