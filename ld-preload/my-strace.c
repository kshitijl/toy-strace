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
  Only do the dlsym lookup once, using a static variable to hold the function pointer.
  Override fopen as well (to catch calls to the C library).
 */
