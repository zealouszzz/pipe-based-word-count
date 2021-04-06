#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include<sys/wait.h>
#include <stdbool.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1
#define DEBUG

bool check_file_exist(char str[]){
    if( access(str, R_OK) == 0 ) {
        return true;
    /*You can also use R_OK, W_OK, and X_OK in place of F_OK to check for read permission, write permission,
    and execute permission (respectively) rather than existence, and you can OR any of them together
    (i.e. check for both read and write permission using R_OK|W_OK)*/
    // file exists
    } else {
    // file doesn't exist
    return false;
    }
}

//get extension of file
char *get_filename_ext(char *filename) {
    char *dot = strrchr(filename, '.');
    return dot + 1;
}

bool check_file_txt(char str[]){
    char *suffix = get_filename_ext(str);
    char txt[] = "txt";
    /*find position where last '.' stays*/
    int t;
    t = strcmp(suffix, txt);
    if(t == 0){
        return true;
    }
    else{
        return false;
    }
}

int count_word(char str[]){
    int i = 0;
    int wrd = 0;
    while(str[i]!='\0'){
        /* check whether the current character is white space or new line or tab character*/
        if(str[i]==' ' || str[i]=='\n' || str[i]=='\t'){
            wrd++;
        }
        i++;
    }
    return wrd;
}

int main ( int argc, char *argv[] ) {
    if ( argc != 2 ) {
        /* We print argv[0] assuming it is the program name */
        printf( "usage: %s, <file_name>\n", argv[0]);
        return 0;
    }
    char read_file[30];
    char file_name[30];
    strncpy(read_file, argv[1],strlen(argv[1]));
    strncpy(file_name, argv[1],strlen(argv[1]));
    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    /*store message for 2nd pipe*/
    int write_msg2;
    int read_msg2;
    pid_t pid;
    int fd[2];   /*sending pipe*/
    int fd2[2];  /*receiving pipe*/
    /* create the pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr,"Sending pipe failed");
        return 1;
    }
    if (pipe(fd2) == -1) {
        fprintf(stderr,"Receving pipe failed");
        return 1;
    }
        /* now fork a child process */
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork failed");
        return 1;
    }

    if (pid > 0) { /* parent process */
        /* close the unused end of the pipe */
        close(fd[READ_END]);
        close(fd2[WRITE_END]);
        /* write to the pipe */
        FILE *fp;
        fp = fopen(file_name, "r"); /*r stands for read only*/
        if(fp == NULL) {
            perror("Error opening file.\n");/*print a custom message before the error message itself.*/
            return 0;
        }
        if(fgets (write_msg, sizeof write_msg, fp)!=NULL) {/*return value is still str*/
        printf("Process 1 is reading file “%s” now ...\n", file_name); /*puts is better for print string*/
        }
        fclose(fp);
        write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
        printf("Process 1 starts sending data to Process 2 ...\n");
        /* close the write end of the pipe */
        close(fd[WRITE_END]);
        wait(NULL); /*wait for child process*/
        read(fd2[READ_END], &read_msg2, sizeof(read_msg2));
        int result = read_msg2;
        printf("The total number of words is %d \n", result);
    }

    else { /* child process */
        /* close the unused end of the pipe */
        close(fd[WRITE_END]);
        close(fd2[READ_END]);
        /* read from the pipe */
        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("Process 2 finishes receiving data from Process 1 ...\n");
        int wrd_num = 0;
        wrd_num = count_word(read_msg);
        printf("Process 2 is counting words now ...\n");

#ifdef DEBUG
    printf("Number of words to be sent to parent process is: %d\n", wrd_num);
#endif
        write_msg2 = wrd_num;
        write(fd2[WRITE_END], &write_msg2, sizeof(write_msg2));
        printf("Process 2 is sending the result back to Process 1 ...\n");
        /* close the write end of the pipe */
        close(fd[READ_END]);
    }
    return 0;
}
