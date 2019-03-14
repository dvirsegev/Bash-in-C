/// dvir segev
/// 318651627

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define LSH_TOK_DELIM " \t\r\n\a"
#define BUFFERSIZE 512

char *Bash_Get_Line(void) {
    char *line = NULL;
    ssize_t bufsize = 0;
    // have getline allocate a buffer for us
    getline(&line, &bufsize, stdin);
    return line;
}

char **Bash_Split_Line(char *line) {
    int bufsize = BUFFERSIZE, position = 0;
    char **theLine = malloc(bufsize * sizeof(char *));
    char *token;

    if (!theLine) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        theLine[position] = token;
        position++;
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    theLine[position] = NULL;
    return theLine;
}

char **removeSign(char **oldArray) {
    char **newargs;
    int place = 0;
    newargs = (char **) malloc(sizeof(oldArray));
    while (strcmp(oldArray[place], "&") != 0) {
        newargs[place] = oldArray[place];
        place++;
    }
    oldArray = newargs;
    free(oldArray);
    return oldArray;
}
/**
 * the cd function!
 * @param pth the orders.
 * @return 1
 */
int cd(char **pth) {
    if (pth[1] == NULL) {
       pth[1] = "/home/dvir";
        // call cd .
        if (chdir(pth[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

/**
 * @param args the line
 * @param exSign 1 if there is & and 0 if there isn't.
 * @return the execute line and return 1.
 */
int Bash_Execute_Program(char **args, int exSign) {
    int stat;
    //pid_t pid = getpid();
    // if there is "&"
    if (exSign) {
        args = removeSign(args);
    }
    // create child
    if (fork() == 0) {
        printf("%ld \n", (long) getpid());
        // check if the code is reasonable.
        int success = execvp(args[0], args);
        if (success < 0) { fprintf(stderr, "Error in system call"); }
        // the parent code
    } else {
        // if there is "&", wait for child to finish
        if (exSign == 0) { wait(&stat); }
        else {
            printf("%ld \n", (long) getppid());
        }
        return 1;
    }
}

/**
 * @param args the input order.
 * @return 1 if there is & in there and 0 if doesn't.
 */
int Bash_getSign(char **args) {
    int place = 0;
    while (args[place] != NULL) {
        // check if there is &
        if (strcmp(args[place], "&") == 0)
            return 1;
        place++;
    }
    return 0;
}
/**
 * @param args the input
 * @return 1 if the order is built int or not
 */
int check_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "exit") == 0)
        return 1;
    return 0;


}

int Bash_Execute_BuiltProgram(char **args) {
    int i=0;
    if (strcmp(args[0], "cd") == 0)
        return cd(args);
    else if (strcmp(args[0], "exit") == 0)
         exit(0);
    else
        printf("Error\n");
    return -1;
}

int main() {
    char *line;
    char **args;
    int exSign, okay = 1;
    while (okay) {
        printf("> ");
        // get line from the user
        line = Bash_Get_Line();
        // split the line
        args = Bash_Split_Line(line);
        // check if there is sign "&".
        exSign = Bash_getSign(args);
        // if the order is built in
        if (check_builtin(args)) {
            printf("%ld \n", (long) getpid());
            //execute the line.
            okay = Bash_Execute_BuiltProgram(args);
        }
        else
            // execute the line. (the order isn't buildup).
            okay = Bash_Execute_Program(args, exSign);
        free(line);
        free(args);
    }

    return 0;
}

