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
/**
 * The dsh function is the brain of the dsh shell and runs basically 
 * all of its functions and controls all the if and else's that come with
 * building a shell
 * 
 * @param cmd the command from the user to be evaluated by the dsh
 * @return int -1 for exit or 0 for keep running
 */
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
            strcat(arguments, " ");
        }   
        i++;
	    space = strtok(NULL, " ");
    }
    if(path[0] != '/') {
        //check if it is a builtin
        if(chkBuiltin(path) == 1) {
            //cd
            char *blankSpace = "\0";
            arguments[strlen(arguments) - 1] = *blankSpace;
            if(strlen(arguments) == 0) {
                //change directory to home if nothing is input with cd
                chdir(getenv("HOME"));
            }
            else {
                //or get the current directory and concat the path onto the end of it
                char pwd[MAXBUF];
                char newPath[MAXBUF] = "";
                if(getcwd(pwd, sizeof(pwd)) != NULL) {
                    //concatenate the newpath to the pwd
                    strcat(newPath, "/");
                    strcat(newPath, arguments);
                    strcat(pwd, newPath);
                }
                if(chdir(pwd) != 0) {
                    //if nothing is there than error message
                    printf("ERROR: No such directory: %s\n", arguments);
                }
            }
        }
        else if(chkBuiltin(path) == 2) {
            //pwd
            char pwd[MAXBUF];
            //print out the current directory or error if not
            if(getcwd(pwd, sizeof(pwd)) != NULL) {
                printf("%s\n", pwd);
            }
            else {
                printf("ERROR: Current directory unable to be found!\n");
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
            //concat the command onto the pwd and try running it
            char pwd[MAXBUF];
            getcwd(pwd, sizeof(pwd));
            strcat(pwd, cmd);
            if (access(path, F_OK | X_OK) == 0) {
                //file exists and is executable
                //if an & is at the end then run the child without waiting
                if(cmd[strlen(cmd) - 1] == '&') {
                    if(fork() == 0) {
                        child(path, arguments);
                    }
                }
                //if it does not have a & then have the parent wait for the child
                else {
                    if(fork() != 0) {
                        parent();
                    }
                    else {
                        child(path, arguments);
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
                                child(newPath, arguments);
                            }
                        }
                        //if it does not have a & then have the parent wait for the child
                        else {
                            if(fork() != 0) {
                                parent();
                            }
                            else {
                                child(newPath, arguments);
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
                    child(path, arguments);
                }
            }
            //if it does not have a & then have the parent wait for the child
            else {
                if(fork() != 0) {
                    parent();
                }
                else {
                    child(path, arguments);
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

/**
 * The parent function is just meant to wait on the child
 */
void parent() {
    wait(NULL);
}

/**
 * The child function takes the path and its arguments and 
 * runs the execv function to run the program that is needed to run
 * 
 * @param path the full path to a directory
 * @param arguments the arguments associated with that call
 */
void child(char* path, char* arguments) {
    char *cmdArgs[MAXBUF];
    char *blankSpace = "\0";
    //create the arguments for the execv
    cmdArgs[0] = path;
    int args = 1;
    if(strlen(arguments) != 0) {
        arguments[strlen(arguments) - 1] = *blankSpace;
        cmdArgs[args] = arguments;
        args++;
    }
    cmdArgs[args] = NULL;
    //run execv
    execv(path, cmdArgs);
}