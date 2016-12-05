/* Author: Cye Stoner

	Ch5, Exercise 5-3:
	This exercise is to verify the atomicity of O_APPEND.
	The following program was ran in to ways:
		$ ./atomic_append file1 1000000 & ./atomic_append file1 1000000
		$ ./atomic_append file2 1000000 x & ./atomic_append file2 1000000 x
	with the following results
		-rw------- 1 cye cye 2000000 Dec  4 21:43 file1
		-rw------- 1 cye cye 1012073 Dec  4 21:43 file2
	The first file was written with O_APPEND, while the second was written
	by calling "lseek(fd, 0, SEEK_END)" before each write.

	As can be clearly seen, atomicity is not guaranteed in the 2nd case.
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void 
errExit(char *output) {
	fprintf(stderr, "%s\n", output);
	exit(EXIT_FAILURE);
}

void
showUsage() {
	errExit("USAGE: atomic_append file num-bytes [x]");
}

int
main(int argc, char *argv[]) {
	int fd;
	off_t offset;
	ssize_t writeNum;
	ssize_t numWritten;

	if (argc < 3 || argc > 4 || strncmp("--help", argv[1]) == 0) {
		showUsage();
	}

	writeNum = (ssize_t) atoll(argv[2]);

	fd = open(argv[1], (argc != 4 ? O_APPEND : 0) | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		errExit("Unable to open file");
	}

	while(writeNum > 0) {
		if (argc == 4) {
			offset = lseek(fd, 0, SEEK_END);
			if (offset == -1) {
				errExit("Could not seek to end of file");
			}
		}
		numWritten = write(fd, "X", 1);
		if(numWritten != 1) {
			errExit("Did not write to file");
		}
		writeNum--;
	}
	
	if (close(fd) == -1) {
		errExit("Closing file did not succeed");
	}

	exit(EXIT_SUCCESS);
}
