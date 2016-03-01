#define _GNU_SOURCE
#include <dlfcn.h>

#include <stdio.h>

int open(const char *pathname, int flags) {
    printf("open(\"%s\", %d)\t", pathname, flags);
    typedef int (*open_func_type)(const char*, int);
    open_func_type original_open = (open_func_type)dlsym(RTLD_NEXT, "open");
    int answer = original_open(pathname, flags);
    printf("= %d\n", answer);
    return answer;
}

/*
  1. Only do the dlsym lookup once, using a static variable to hold the function pointer.
  2. Override fopen as well (to catch calls to the C library).
  3. C++ name mangling gets in the way, so either write a .c file or do extern "C"
  4. Use nm to inspect all the symbols in a binary.
  5. Use LD_DEBUG=all to print out debug info about which symbol was looked up where.
 */
