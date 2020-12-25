#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//@author Ariel Traver

/*** @function error
* takes an input from some function and prints the appropriate error message, then quits.
* parameters:
    * errornum, the error code
***/
int error(int errornum) {
    switch (errornum) {
        case 1: 
            fprintf(stderr, "cdocs format: cdocs <program filepath> <readme filepath> <optional:delim1> <optional:delim2>\n");
            exit(1);
        case 2:
            fprintf(stderr, "Failed to open file\n");
            exit(2);
        case 3:
            fprintf(stderr, "fseek failed\n");
            exit(3);
        case 4:
            fprintf(stderr, "fread failed\n");
            exit(4);
        case 5:
            fprintf(stderr, "Could not open program file\n");
            exit(5);
        case 6:
            fprintf(stderr, "fork failed\n");
            exit(6);
        case 7:
            fprintf(stderr, "dup2 failed\n");
            exit(7);
        default:
            fprintf(stderr, "Unknown error\n");
            exit(-1);
    }
    return 0;
}

/***@function init_readme
* opens a file where the readme will be written.
* overwrites any existing readme files with the same name.
***/
FILE* init_readme(char* filename) {
    FILE* readme;
    if(filename) {
        readme = fopen(filename, "w");
    } 
    else { readme = fopen("README.md", "w"); }
    if (!readme) {error(2);}
    return readme;
}

FILE* open_program(char* program_name) {
    FILE* program = fopen(program_name, "r");
    if (!program) { error(5);}
    return program;
}

int filesize(FILE* file) {
    if (fseek(file, 0, SEEK_END)) {error(3);}
    int filesize = ftell(file);
    rewind(file);
    return filesize;
}

char* load_buffer(FILE* program){
    int length = filesize(program);
    char* buffer = malloc(sizeof(char) * (length + 1));
    if (!fread(buffer, sizeof(char), length, program)) {
        free(buffer);
        fclose(program);
        error(4);
    }
    return buffer;
} 

char* clean_name(char* program_name) {
    int length = strlen(program_name);
    char* name = malloc(sizeof(char) * length);
    strncpy(name, program_name, length);
    char* dot = strstr(name, ".");
    if (dot) {
        *dot = '\0'; //cut off the .c, .py, etc
    }
    return name;
}


int print_author(char* buffer) {
    //get the author's name from line @author
    char* author = strstr(buffer, "@author") + 8; //find the author line
    strtok(author, "\n");
    if (author >= 0) {
        printf("## Author: %s\n", author);
    }
    author[strlen(author)] = '9';
    return 0;
}

/***@function print_headers
* Prints all the lines contained within the specified delimeters
* Parameters:
    * buffer, the buffer containing text file contents
    * opn_delim, provided opening delimeter or none
    * cls_delim, provided closing delimeter or none
***/
int print_headers(char* buffer, char* opn_delim, char* cls_delim) {
    char* end = strlen(buffer) + buffer;
    char* cursor = strstr(buffer, opn_delim);
    char* close;
    char* title;
    while(cursor < end && cursor) {
        close = strstr(cursor, cls_delim);
        if(close) {*close = '\0';}
        if((cursor-1<=buffer) || (*(cursor-1) != '\"')){
            cursor+=strlen(opn_delim);
            title = strstr(cursor, "@function");
            if(title) { //nicely format the function title
                printf("### Function:");
                cursor = title + 9;
            }
            printf("%s\n", cursor);
        }
        cursor+=(strlen(cursor)+1);
        if(cursor < end) { cursor = strstr(cursor, opn_delim);}
    }
    return 0;
}


int write_readme(int argc, char** argv, FILE* readme, FILE* program) {
    
    int status;
    pid_t pid = fork();
    if (pid < 0) {
        fclose(program);
        fclose(readme);
        error(6);
    }
    if (pid == 0) {
        
        if(dup2(fileno(readme), STDOUT_FILENO) < 0) {
            fclose(program);
            fclose(readme);
            error(7);
        }

        char* buffer = load_buffer(program); //load file into char array
        char* opn_delim = "/***";
        char* cls_delim = "***/";

        if(argc >=3) { opn_delim = *(argv + 3); }
        if(argc>=4) { cls_delim = *(argv + 4); }
       
        char* name = clean_name(*(argv+1)); //get rid of .c / .py / etc
        printf("# %s\n", name); //big title in markdown

        free(name); //no need for this
        print_author(buffer);
        print_headers(buffer, opn_delim, cls_delim);
       
	free(buffer); //free this child's version of buffer
	exit(0);
    }    //get the author's name from line
    else { //we are a parent
        if (!waitpid(pid, &status, 0)) {
            error(8);
        }
    }
    return 0;
}

int handle_input(int argc, char** argv){
    FILE* readme = init_readme(*(argv + 2));
    FILE* program = open_program(*(argv + 1));
    write_readme(argc, argv, readme, program);
    fclose(program);
    fclose(readme);
    return 0;
}

int main(int argc, char** argv) {
    if(argc < 2) { error(1); }
    handle_input(argc, argv);
    exit(0);
    return 0;
}
