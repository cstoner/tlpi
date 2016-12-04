/* Author: Cye Stoner

This code solves exercies 4-1 of The Linux Programming Interface
It re-creates the functionality of the 'tee' program, using system calls

USAGE: tee [-a] src_file
*/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 500

void showUsage(const char *name) {
	fprintf(stderr, "Usage: %s [-a] file_in file_out\n", name);
	exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[]) {
	int opt, fd_in, fd_out;
	char *buf;
	mode_t filePerms;
	ssize_t numRead;
	bool append = false;

	while ((opt = getopt(argc, argv, "a")) != -1) {
		switch(opt) {
		case 'a':
			append = true;
		}
	}

	if (argc - optind != 2) {
		showUsage(argv[0]);
	}

	buf = malloc(BUF_SIZE);
	if (buf == NULL) {
		fprintf(stderr, "Unable to malloc buffer\n");
		exit(EXIT_FAILURE);
	}

	fd_in = open(argv[optind], O_RDONLY);
	if (fd_in == -1) {
		fprintf(stderr, "Unable to open file '%s'\n", argv[optind]);
		exit(EXIT_FAILURE);
	}

	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH;
	fd_out = open(argv[optind+1], O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), filePerms);
	while ((numRead = read(fd_in, buf, BUF_SIZE)) > 0) {
		printf("%.*s", (int)numRead, buf);
		if (write(fd_out, buf, numRead) != numRead) {
			fprintf(stderr, "Did not write whole buffer\n");
			exit(EXIT_FAILURE);
		}
	}
	if (numRead == -1) {
		fprintf(stderr, "Error reading input file\n");
		exit(EXIT_FAILURE);
	}


	if (close(fd_out) < 0) {
		fprintf(stderr, "error closing file handle for %s\n", argv[optind+1]);
		exit(EXIT_FAILURE); 
	}
	if (close(fd_in) < 0) {
		fprintf(stderr, "error closing file handle for %s\n", argv[optind]);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
