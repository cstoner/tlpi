/* Author: Cye Stoner

	ch6 exercise 6-1.
	Write a program to see what happens if we try to longjmp() into a function
	that has already returned.

	Answer: It causes a segmentation fault
*/

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

static jmp_buf env;

void
someFunction() {
	setjmp(env);

	printf("Inside of someFunction()\n");
}

int
main(int argc, char *argv[]) {
	printf("Before someFunction()\n");
	someFunction();
	printf("After someFunction()\n");

	printf("Calling longjmp()\n");
	longjmp(env, 1);
	printf("After longjmp()\n");
	exit(EXIT_SUCCESS);
}
