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
#include <stdbool.h>
#include "builtins.h"

char history[HISTORY_LEN] = "";

// TODO: Your function definitions (declarations in dsh.h)
int dsh(char* cmd) {
    char path[MAXBUF] = "";
    //create the history timeline
    strcat(history, cmd);
    strcat(history, "\n");
    strcpy(path, cmd);
    char arguments[MAXBUF] = "";
    char *space = strtok(path, " ");
    int i = 0;
    //split the string by spaces and place them into the path or make an argument
	while (space != NULL)
	{
        if(i == 0) {
            //puts everything before first space into the path
            strcpy(path, space);
        }
        else {
            //creates the arguments
            strcat(arguments, space);
            strcat(arguments, "\0");
        }   
        i++;
	    space = strtok(NULL, " ");
    }
    if(path[0] != '/') {
        //check if it is a builtin
        if(chkBuiltin(path) == 1) {
            //cd
            if(strlen(arguments) == 0) {
                chdir(getenv("HOME"));
            }
            else {
                char pwd[MAXBUF];
                char newPath[MAXBUF] = "";
                if(getcwd(pwd, sizeof(pwd)) != NULL) {
                    //concatenate the newpath to the pwd
                    strcat(newPath, "/");
                    strcat(newPath, arguments);
                    strcat(pwd, newPath);
                }
                if(chdir(pwd) != 0) {
                    printf("No such directory: %s\n", arguments);
                }
            }
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
            char *space = strtok(history, "\n");
            //split the string by spaces and place
	        while (space != NULL)
	        {
                printf("%s\n", space);
	            space = strtok(NULL, "\n");
            }
        }
        else if(chkBuiltin(path) == 4) {
            //exit
            return -1;
        }
        
        //if not a builtin
        else {
            char pwd[MAXBUF];
            getcwd(pwd, sizeof(pwd));
            strcat(pwd, cmd);
            if (access(path, F_OK | X_OK) == 0) {
                //file exists and is executable
                //if an & is at the end then run the child without waiting
                if(cmd[strlen(cmd) - 1] == '&') {
                    if(fork() == 0) {
                        child(path, arguments, i);
                    }
                }
                //if it does not have a & then have the parent wait for the child
                else {
                    if(fork() != 0) {
                        parent();
                    }
                    else {
                        child(path, arguments, i);
                    }
                }
            }
            else {
                char pathenv[MAXBUF] = "";
                strcpy(pathenv, getenv("PATH"));
                char *colon = strtok(pathenv, ":");
                //create a bool that will show in the end if it is a file or not
                bool fileThere = true;
                //split the string by : and check if it is a executable
	            while (colon != NULL)
	            {
                    //make a new path char
                    char newPath[MAXBUF] = "";
                    strcpy(newPath, colon);
                    //cat the path into it to make it into a potential file that is executable
                    strcat(newPath, "/");
                    strcat(newPath, path);
                    if (access(newPath, F_OK | X_OK) == 0) {
                        //file exists and is executable
                        //if an & is at the end then run the child without waiting
                        if(cmd[strlen(cmd) - 1] == '&') {
                            if(fork() == 0) {
                                child(newPath, arguments, i);
                            }
                        }
                        //if it does not have a & then have the parent wait for the child
                        else {
                            if(fork() != 0) {
                                parent();
                            }
                            else {
                                child(newPath, arguments, i);
                            }
                        }
                        fileThere = true;
                        break;
                    }
                    else {
                        fileThere = false;
                        colon = strtok(NULL, ":");
                    }
                }
                if(!fileThere) {
                    // file doesn't exist or is not executable
                    printf("ERROR: %s not found!\n", path);
                }
            }
        }
    }
    else {
        if (access(path, F_OK | X_OK) == 0) {
            //file exists and is executable
            //if an & is at the end then run the child without waiting
            if(cmd[strlen(cmd) - 1] == '&') {
                if(fork() == 0) {
                    child(path, arguments, i);
                }
            }
            //if it does not have a & then have the parent wait for the child
            else {
                if(fork() != 0) {
                    parent();
                }
                else {
                    child(path, arguments, i);
                }
            }
        }
        else {
            // file doesn't exist or is not executable
            printf("ERROR: %s not found!\n", path);
        }
    }
    return 0;
}

void parent() {
    wait(NULL);
}

void child(char* path, char* arguments, int size) {
    char *cmdArgs[MAXBUF];
    cmdArgs[0] = path;
    int arg = 1;
    char *space = strtok(arguments, "\0");
    //split the string by spaces and place them into the path or make an argument
	while (space != NULL)
	{
        cmdArgs[arg] = space;
        arg++;
        space = strtok(NULL, "\0");
    }
    cmdArgs[arg] = NULL;
    //run execv
    execv(path, cmdArgs);
}