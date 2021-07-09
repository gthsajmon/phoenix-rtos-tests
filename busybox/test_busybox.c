#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int ret;
	DIR* dir = opendir("/bin/testsuite");

	if (dir) {
		closedir(dir);
	} else if (ENOENT == errno) {
		fprintf(stderr, "There is no busybox test suite to run, build project with \"long_test\"\n");
		return 1;
	} else {
		fprintf(stderr, "There is problem with opening existing /bin/testsuite directory\n");
		return 1;
	}

	if ((ret = vfork()) < 0) {
		fprintf(stderr, "vfork function failed \n");
		return 1;
	}
	else if (!ret) {
		execv("/bin/posixsrv", argv);
		fprintf(stderr, "exec function failed \n");
		return 1;
	}
	/* without this delay sometimes posixsrv is not yet running before the next function call */
	usleep(500000);

	if ((ret = system("cd /bin/testsuite/ && export PATH=/bin:/sbin:/usr/bin:/usr/sbin && ./runtest")) < 0) {
		fprintf(stderr, "system function failed \n");
		return 1;
	}

	printf("\nRuntest done\n\n");

	return 0;
}
