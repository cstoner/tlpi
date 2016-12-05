/* Author: Cye Stoner

	ch5, exercises 5-4, 5-5, and 5-6

	5-4: Re-write dup and dup2 using fctnl() and close().
	5-5: Verify that duplicated file descriptors share a file offset value
	     nd open file status flags
	5-6: Questions about the behavior of duplicated fd after several writes

	fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | IS_IWUSR)
	fd2 = dup(fd1)
	fd3 = open(file, O_RDWR)

	write(fd1, "Hello,", 6) 
	- File contains "Hello," because O_TRUNC truncated the file to be empty
	write(fd2, " world", 6)
	- File contains "Hello, world" because the offset is shared between fd1 and fd2
	lseek(fd2, 0, SEEK_SET)
	write(fd1, "HELLO,", 6)
	- File contains "HELLO, world" because the lseek on fd2 also reset fd1 to the same value
	write(fd3, "Gidday", 6)
	- File contains "Gidday world" because the offset of fd3 is not shared with fd1 and fd2
*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void errExit(const char *output) {
	fprintf(stderr, "%s\n", output);
	exit(EXIT_FAILURE);
}

int
my_dup(const int oldfd) {
	int newfd;
	newfd = fcntl(oldfd, F_DUPFD, 0);
	return newfd;
}

int
my_dup2(const int oldfd,const int newfd) {
	int tmpfd;

	if (fcntl(oldfd, F_GETFL) == -1) {
		errno = EBADF;
		return -1;
	}

	if (oldfd == newfd) {
		return newfd;
	}
	
	if (fcntl(newfd, F_GETFL) != -1) {
		close(newfd);
	}
	tmpfd = fcntl(oldfd, F_DUPFD, newfd);
	
	return tmpfd;	
}

int main(int argc, char *argv[]) {
	int fd1, fd2, fd3;
	off_t fd1_off, fd2_off, fd3_off;
	int fd1_flags, fd2_flags;

	const char *testFile = "dup_testfile.txt";

	fd1 = open(testFile, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
	if (fd1 == -1) {
		errExit("Unable to open testfile");
	}

	fd2 = my_dup(fd1);
	if (fd2 == -1) {
		errExit("my_dup failed");
	}

	fd1_off = lseek(fd1, 0, SEEK_CUR);
	fd2_off = lseek(fd2, 0, SEEK_CUR);
	
	if (fd1_off != fd2_off) {
		errExit("Initial offset of fd1 and fd2 are not the same");
	}

	// BEGIN 5-5 checking
	lseek(fd1, 1, SEEK_CUR);
	fd1_off = lseek(fd1, 0, SEEK_CUR);
	fd2_off = lseek(fd2, 0, SEEK_CUR);

	if (fd1_off != fd2_off) {
		errExit("Offset of fd1 and fd2 are not the same after seek");
	} else {
		printf("Offset value after seek: %lld\n", (long long) fd1_off);
	}

	fd1_flags = fcntl(fd1, F_GETFL);
	fd2_flags = fcntl(fd2, F_GETFL);
	if (fd1_flags != fd2_flags) {
		errExit("Open file flags for fd1 and fd2 are not equal");
	}
	// END 5-5 checking

#define FD3_NUM 9
	fd3 = my_dup2(fd1, FD3_NUM);
	if (fd3 != FD3_NUM) {
		errExit("fd3 not using requested fd number");
	}
	fd3_off = lseek(fd3, 0, SEEK_CUR);
	if (fd3_off != fd1_off) {
		errExit("fd3 not pointing to same file descrption as fd1");
	}

	exit(EXIT_SUCCESS);
}
