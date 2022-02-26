/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include "dsh.h"

int main(int argc, char **argv)
{
	char cmdline[MAXBUF]; // stores user input from commmand line
	
	//get motd
	char motd[MAXBUF];
	strcpy(motd, getenv("HOME"));
	strcat(motd, "/.dsh_motd");
	bool dshOn = true;
	int test;
	while(dshOn) {
		//run the dsh until exit is returned
		printf("dsh> ");
		fgets(cmdline, MAXBUF, stdin);
		cmdline[strlen(cmdline) - 1] = '\0';
		test = dsh(cmdline);
		if(test == -1) {
			dshOn = false;
		}
		printf("\n");
	}
	return 0;
}
