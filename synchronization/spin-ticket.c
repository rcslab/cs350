#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>

_Atomic int lock = 0;
_Atomic int next = 0;
const int N = 1000000;

int last = -1;
int unfair;

void spin_lock(void) {
	int ticket = atomic_fetch_add(&next, 1);

	while (atomic_load(&lock) != ticket) {
		// Spin!
	}
}

void spin_unlock(void) {
	atomic_fetch_add(&lock, 1);
}

void *func(void *ptr) {
	int which = *(int *)ptr;

	for (int i = 0; i < N; ++i) {
		spin_lock();
		if (last == which) {
			++unfair;
		}
		last = which;
		spin_unlock();
	}

	return NULL;
}

int main(void) {
#define NTHREADS 4
	pthread_t threads[NTHREADS];
	int ids[NTHREADS];

	for (int i = 0; i < NTHREADS; ++i) {
		ids[i] = i;
		pthread_create(&threads[i], NULL, func, &ids[i]);
	}
	for (int i = 0; i < NTHREADS; ++i) {
		pthread_join(threads[i], NULL);
	}
	printf("unfair: %d\n", unfair);
	return 0;
}
