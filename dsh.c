/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>
#include "builtins.h"

char history[HISTORY_LEN] = "";

// TODO: Your function definitions (declarations in dsh.h)
int dsh(char* cmd) {
    char path[MAXBUF];
    strcat(history, cmd);
    strcat(history, " ");
    printf("%s\n", history);
    // char arguments[MAXBUF] = "";
    // char *space = strtok(cmd, " ");
    // int i = 0;
    //split the string by spaces and place them into the path or make an argument
	// while (space != NULL)
	// {
    //     if(i == 0) {
    //         //puts everything before first space into the path
    //         strcpy(path, cmd);
    //     }
    //     else {
    //         //creates the arguments
    //         strcat(arguments, cmd);
    //         strcat(arguments, " ");
    //     }   
    //     i++;
	//     space = strtok(NULL, " ");
    // }
    strcpy(path, cmd);
    if(path[0] != '/') {
        //check if it is a builtin
        if(chkBuiltin(path) == 1) {
            //cd
            chdir("..");
        }
        else if(chkBuiltin(path) == 2) {
            //pwd
            char pwd[MAXBUF];
            if(getcwd(pwd, sizeof(pwd)) != NULL) {
                printf("%s\n", pwd);
            }
            else {
                printf("Current directory unable to be found!\n");
            }
        }
        else if(chkBuiltin(path) == 3) {
            //history
            char *space = strtok(history, " ");
            //split the string by spaces and place
	        while (space != NULL)
	        {
                printf("%s\n", history);
	            space = strtok(NULL, " ");
            }
        }
        else if(chkBuiltin(path) == 4) {
            //exit
            return -1;
        }
        else if(chkBuiltin(path) == 5) {
            //echo
            return -1;
        }
    }
    else {
        if (access(path, F_OK | X_OK) == 0) {
            // file exists and is executable
            // if(path[strlen(path)] == '&') {
            //     if(fork() == 0) {
            //         child(path);
            //     }
            // }
            // else {
                if(fork() != 0) {
                    parent();
                }
                else {
                    child(path);
                }
            // }
        }
        else {
            // file doesn't exist or is not executable
            printf("Error not a exec\n");
        }
    }
    return 0;
}

void parent() {
    wait(NULL);
}

void child(char* path) {
    char *cmdArgs[2];
    cmdArgs[0] = path;
    // int arg = 1;
    // if (!strcmp(arguments, "")) {
    //     char *space = strtok(arguments, " ");
    //     //split the string by spaces and place them into the path or make an argument
	//     while (space != NULL)
	//     {
    //         cmdArgs[arg] = arguments;
    //         arg++;
    //         space = strtok(NULL, " ");
    //     }
    // }
    cmdArgs[1] = NULL;
    execv(path, cmdArgs);
}