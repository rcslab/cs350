#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

#define NFIBERS 4

ucontext_t fibers[NFIBERS];
int done[NFIBERS];
int current;

void fiber_yield(void) {
	int prev = current;

	for (int i = 0; i < NFIBERS; ++i) {
		current += 1;
		current %= NFIBERS;

		if (!done[current]) {
			if (current != prev) {
				swapcontext(&fibers[prev], &fibers[current]);
			}
			break;
		}
	}
}

void fiber_exit(void) {
	done[current] = 1;
	fiber_yield();
}

void fiber_fn(int start, int end) {
	int sum = 0;

	for (int i = start; i < end; ++i) {
		sum += i;
		if (i == (start + end) / 2) {
			printf("sum: %d (yield)\n", sum);
			fiber_yield();
		}
	}

	printf("sum: %d\n", sum);
	fiber_exit();
}

int main(void) {
	for (int i = 0; i < NFIBERS; ++i) {
		getcontext(&fibers[i]);
		fibers[i].uc_stack.ss_sp = malloc(SIGSTKSZ);
		fibers[i].uc_stack.ss_size = SIGSTKSZ;
		fibers[i].uc_link = NULL;
		makecontext(&fibers[i], (void (*)())fiber_fn, 2, 100 * i, 100 * (i + 1));
	}

	setcontext(&fibers[0]);
	return 0;
}
