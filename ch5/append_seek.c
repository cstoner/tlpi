/* Author: Cye Stoner

	Ch5, Exercise 5-2:
	Write a program that opens an existing file for writing with O_APPEND
	and then seek to the beginning before writing.
	Where does the data appear in the file? Why?

	The data are appended to the file.
	It's not entirely clear to me why lseek claims to have gone to offset
	0, while SEEK_CUR correctly reports the end of the file, but the reasoning
	for the behavior is clearly that O_APPEND is enforcing the "append only"
	behavior.
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
	errExit("USAGE: append_seek file");
}

int
main(int argc, char *argv[]) {
	int fd;
	off_t offset;
	ssize_t numWritten;

	if (argc != 2 || strncmp("--help", argv[1]) == 0) {
		showUsage();
	}

	fd = open(argv[1], O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

	if (fd == -1) {
		errExit("Unable to open file for append");
	}

	offset = lseek(fd, 0, SEEK_SET); 
	if (offset != 0) {
		fprintf(stderr, "Seeked to offset %lld\n", (long long) offset);
		errExit("Unable to seek to offset 0");
	}

	numWritten = write(fd, "Hello, World!\n", 14);
	if (numWritten != 14) {
		errExit("Was not able to write full buffer to file");
	}

	offset = lseek(fd, 0, SEEK_CUR);
	printf("Current offset: %lld\n", (long long)offset);

	if (close(fd) == -1) {
		errExit("Closing file did not succeed");
	}

	exit(EXIT_SUCCESS);
}
