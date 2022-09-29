#include <stdio.h>
#include <stdlib.h>

// tries to convert the first argument to an int and prints it
int main (int argc, char *argv[]) {
	if (argc <= 1) {
		printf("exiting ...\n");
		exit(1);
	}
	int arg1 = atol(argv[1]);
	printf("The number: %d\n", arg1);
	return 0;
}
