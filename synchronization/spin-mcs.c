#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct qnode qnode;

struct qnode {
	_Atomic(qnode *) next;
	_Atomic bool locked;
};

_Atomic(qnode *) lock = NULL;

const int N = 100000;

int last = -1;
int unfair;

void spin_lock(qnode *node) {
	atomic_init(&node->next, NULL);
	atomic_init(&node->locked, true);

	qnode *prev = atomic_exchange(&lock, node);
	if (prev) {
		atomic_store(&prev->next, node);
		while (atomic_load(&node->locked)) {
			// Spin!
		}
	}
}

void spin_unlock(qnode *node) {
	if (!atomic_load(&node->next)) {
		qnode *prev = node;
		if (atomic_compare_exchange_strong(&lock, &prev, NULL)) {
			return;
		}
	}

	qnode *next;
	do {
		next = atomic_load(&node->next);
	} while (!next);

	atomic_store(&next->locked, false);
}

void *func(void *ptr) {
	int which = *(int *)ptr;

	for (int i = 0; i < N; ++i) {
		qnode node;
		spin_lock(&node);
		if (last == which) {
			++unfair;
		}
		last = which;
		spin_unlock(&node);
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
