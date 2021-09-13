#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	int ret;
	DIR *dir = opendir("/bin/testsuite");

	if (dir) {
		closedir(dir);
	}
	else if (ENOENT == errno) {
		fprintf(stderr, "There is no busybox test suite to run, build project with \"long_test\"\n");
		return (1);
	}
	else {
		fprintf(stderr, "There is problem with opening existing /bin/testsuite directory: %s\n", strerror(errno));
		return (1);
	}
	if ((ret = vfork()) < 0) {
		fprintf(stderr, "vfork function failed: %s\n", strerror(errno));
		return (1);
	}
	else if (!ret) {
		execv("/bin/posixsrv", argv);
		fprintf(stderr, "exec function failed: %s\n", strerror(errno));
		_exit(EXIT_FAILURE);
	}
	/* without this delay sometimes posixsrv is not yet running before the next function call */
	usleep(100000);
	if (waitpid(ret, NULL, WNOHANG) > 0) {
		fprintf(stderr, "Posixsrv failed to run in background\n");
		return 1;
	}
	else
		printf("Posixsrv ran in background\n");

	if ((ret = system("cd /bin/testsuite/ && export PATH=/bin:/sbin:/usr/bin:/usr/sbin && ./runtest -v")) < 0) {
		fprintf(stderr, "system function failed: %s\n", strerror(errno));
		return (1);
	}

	printf("\nRuntest done\n\n");

	return 0;
}
