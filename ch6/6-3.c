/* Author: Cye Stoner

	ch6 exercise 6-3
	Implement setenv() and unsetenv() using getenv(), putenv(), and when necessary,
	code that directly modifies envion.
*/

#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

void errExit() {
	fprintf(stderr, "Exiting...\n");
	exit(EXIT_FAILURE);
}

int
my_unsetenv(const char *name) {
	char **ep;
	bool fill_hole, unset_something;
	char *buf;
	char *name_cmp;
	const int var_len = strlen(name)+1;

	name_cmp = (char *)malloc(var_len+1);
	sprintf(name_cmp, "%s=", name);

	unset_something = false;

	while((buf = getenv(name)) != NULL) {
		fill_hole = false;
		for(ep = environ; *ep != NULL; ep++) {
			if(fill_hole) {
				*ep = *(ep+1);
			} else {
				if (strncmp(*ep, name_cmp, var_len) == 0) {
					free(*ep);
					*ep = NULL;
					fill_hole = true;
					unset_something = true;
				}
			}
		}
		if(!unset_something) {
			return -1;
		}
	}
	return unset_something ? 0 : -1;
}

int
my_setenv(const char *name, const char *value, int overwrite) {
	int name_len, val_len, total_len;
	int status;
	char *buf;

	name_len = strlen(name);
	val_len = strlen(value);
	total_len = name_len + val_len + 2; // Add 2 bytes for '=' and '\0'

	if(overwrite == 0) {
		buf = getenv(name);
		if (buf != NULL) return 0;
	} else {
		my_unsetenv(name);
	}

	buf = (char *)malloc(total_len);
	if (buf == NULL) {
		perror("setenv_malloc");
		return -1;
	}
	status = sprintf(buf, "%s=%s", name, value);
	if (status != total_len-1) {
		perror("setenv_sprintf");
		return -1;
	}

	putenv(buf);
	return 0;
}

int
main(int argc, char *argv[]) {
	int status;
	const char *myvar = "SETENV_TEST";
	char *buf;

	status = my_setenv(myvar, "1", 0);
	if(status != 0) {
		fprintf(stderr, "Unable to setenv\n");
		errExit();
	} 
	buf = getenv(myvar);
	if(buf == NULL) {
		fprintf(stderr, "env variable not found\n");
		errExit();
	} 
	printf("Enviornment variable set: %s\n", buf);

	status = my_setenv(myvar, "1234", 1);
	if(status != 0) {
		fprintf(stderr, "Unable to setenv\n");
		errExit();
	} 
	buf = getenv(myvar);
	if(buf == NULL) {
		fprintf(stderr, "env variable not found\n");
		errExit();
	} 
	printf("Enviornment variable overwritten: %s\n", buf);

	status = my_unsetenv(myvar);
	if (status == -1) {
		fprintf(stderr, "Could not unset variable\n");
		errExit();
	}
	buf = getenv(myvar);
	if (buf != NULL) {
		fprintf(stderr, "Env variable somehow not unset\n");
		errExit();
	}

	exit(EXIT_SUCCESS);
}
