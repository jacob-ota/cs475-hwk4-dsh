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

	printf("MOTD: %s\n", getenv("HOME"));
	bool dshOn = true;
	int test;
	while(dshOn) {
		printf("dsh> ");
		scanf("%s", cmdline);
		test = dsh(cmdline);
		if(test == -1) {
			dshOn = false;
		}
	}
	return 0;
}
