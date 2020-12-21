#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int error(int errornum) {
    switch (errornum) {
        case 1: 
            fprintf(stderr, "cdocs format: cdocs <program filepath> <readme filepath>\n");
            exit(1);
        case 2:
            fprintf(stderr, "Failed to open file\n");
            exit(2);
    }
    return 0;
}

int init_readme(char** argv) {
    FILE* readme;
    if(*(argv + 2)) {
        readme = fopen(*(argv+2), "w");
    } 
    else { readme = fopen("README.md", "w"); }
    if (!readme) {
        error(2);
    }
    fclose(readme);
    return 0;
}

int main(int argc, char** argv) {
    if(argc < 2) { error(1); }
    init_readme(argv);
    exit(0);
    return 0;
}
