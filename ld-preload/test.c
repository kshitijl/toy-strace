#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *f = fopen("./build", "r");
    fclose(f);
    return 0;
}
