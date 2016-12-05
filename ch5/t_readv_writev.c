/* Author: Cye Stoner

	ch5 exercise 5-7
	Implement readv() and writev() using read(), write() and malloc
*/

#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

void
errExit(const char *output) {
	fprintf(stderr, "%s\n", output);
	exit(EXIT_FAILURE);
}

ssize_t
my_readv(int fd, struct iovec *iov, int iovcnt) {
	ssize_t numRead = 0;
	ssize_t tmpRead;
	int i;
	for (i = 0; i < iovcnt; i++) {
		iov[i].iov_base = malloc(iov[i].iov_len);
		tmpRead = read(fd, iov[i].iov_base, iov[i].iov_len);
		if (tmpRead == -1) {
			errExit("fewer bytes read than requested");
		} else if (tmpRead == 0) {
			return 0;
		}
		numRead += tmpRead;
	}
	return numRead; 
}

ssize_t
my_writev(int fd, const struct iovec *iov, int iovcnt) {
	ssize_t numWritten = 0;
	ssize_t tmpWritten;
	int i;
	for (i = 0; i < iovcnt; i++) {
		tmpWritten = write(fd, iov[i].iov_base, iov[i].iov_len);
		if (tmpWritten != iov[i].iov_len) {
			errExit("fewer bytes written than requested");
		}
		numWritten += tmpWritten;
	}
	return numWritten;
}

int
main() {
	int fd;
	struct iovec iov_w[3];
	struct iovec iov_r[3];
	int totRequired, bytesWritten, bytesRead;
	// Defaults to write
	char *buf = "Hello, World!";
	int x = 4096;
	long long y = 1000000;
	// Place to store read values
	char read_buf[14];
	int read_x;
	long long read_y;

	fd = open("test_readv_writev.data", O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
	if (fd == -1) {
		errExit("Unable to open file");
	}

	iov_w[0].iov_base = buf;
	iov_w[0].iov_len = strlen(buf);
	iov_r[0].iov_len = strlen(buf);
	totRequired = iov_w[0].iov_len;

	iov_w[1].iov_base = &x;
	iov_w[1].iov_len = sizeof(x);
	iov_r[1].iov_len = sizeof(x);
	totRequired += iov_w[1].iov_len;

	iov_w[2].iov_base = &y;
	iov_w[2].iov_len = sizeof(y);
	iov_r[2].iov_len = sizeof(y);
	totRequired += iov_w[2].iov_len;

	bytesWritten = my_writev(fd, iov_w, 3);
	if(totRequired != bytesWritten) {
		errExit("Bytes written different than number required");
	}
	printf("Wrote %lld bytes\n", (long long)bytesWritten);

	if(lseek(fd, 0, SEEK_SET) != 0) {
		errExit("Error resetting file offset to 0");
	}

	bytesRead = my_readv(fd, iov_r, 3);
	if(totRequired != bytesWritten) {
		errExit("Bytes read different than number required");
	}
	printf("Read %lld bytes\n", (long long)bytesRead);

	if (strncmp((char*)iov_w[0].iov_base, (char*)iov_r[0].iov_base, iov_w[0].iov_len) != 0) {
		errExit("String value read not equal to value written");
	}
	if (*(int*)iov_w[1].iov_base != *(int*)iov_r[1].iov_base) {
		errExit("Integer value read not equal to value written");
	}
	if (*(long long*)iov_w[2].iov_base != *(long long*)iov_r[2].iov_base) {
		errExit("Long long value read not equal to value written");
	}

	exit(EXIT_SUCCESS);
}
