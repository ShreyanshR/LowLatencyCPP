//
// Created by Shreyansh Rathore on 01/03/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include "common.h"

int main(int argc, char* argv[]) {

	int *p = malloc(sizeof(int));
	assert(p != NULL);
	printf("(%d) address of p: %08x\n", getpid(),(unsigned) p);
	*p = 0;
	/*
	if (argc != 2) {
		fprintf(stderr, "usage: cpu <string>\n");
		exit(1);
	}
	*/
	//char* str = argv[1];
	while (1) {
		Spin(1);
		*p = *p + 1;
		//printf("%s\n", str);(%d
		printf("(%d) p: %d\n", getpid(), *p);
	}

	return 0;
}