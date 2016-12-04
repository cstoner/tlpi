/* Author: Cye Stoner

This code solves exercies 4-2 of The Linux Programming Interface
It creates a simple version of 'cp' that, when asked to copy a file
with holes, preserves the holes in the destination file.

USAGE: cp_null src_file dest_file
*/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 500

void showUsage(const char *name) {
	fprintf(stderr, "Usage: %s src_file dest_file \n", name);
	exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[]) {
	int opt, fd_in, fd_out;
	char buf;
	mode_t filePerms;
	ssize_t numRead;

	if (argc != 3) {
		showUsage(argv[0]);
	}

	fd_in = open(argv[1], O_RDONLY);
	if (fd_in == -1) {
		fprintf(stderr, "Unable to open file '%s' for reading\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH;
	fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, filePerms);
	if (fd_out == -1) {
		fprintf(stderr, "Unable to open file '%s' for writing\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	while ((numRead = read(fd_in, &buf, 1)) == 1) {
		if (buf == '\0') {
			lseek(fd_out, 1, SEEK_CUR);
		} else if (write(fd_out, &buf, numRead) != numRead) {
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
