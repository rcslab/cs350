#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
    int status;
    int x = 1;

    printf("%d\n", x);
    status = fork();
    if (status == 0) {
	x += 1;
    } else {
	x *= 2;
	waitpid(status, NULL, 0);
    }
    printf("%d\n", x);

    status = fork();
    if (status == 0) {
	x *= 2;
    } else {
	x *= 3;
    }
    printf("%d\n", x);
}
