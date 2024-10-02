#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>

_Atomic bool lock = false;
const int N = 100000;

int last = -1;
int unfair;

void spin_lock(void) {
	while (atomic_exchange(&lock, true)) {
		while (atomic_load(&lock)) {
			// Spin!
		}
	}
}

void spin_unlock(void) {
	atomic_store(&lock, false);
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
