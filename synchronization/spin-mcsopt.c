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

	qnode *prev = atomic_exchange_explicit(&lock, node, memory_order_acq_rel);
	if (prev) {
		atomic_store_explicit(&prev->next, node, memory_order_release);
		atomic_thread_fence(memory_order_acq_rel);
		while (atomic_load_explicit(&node->locked, memory_order_acquire)) {
			// Spin!
		}
	}
}

void spin_unlock(qnode *node) {
	qnode *next = atomic_load_explicit(&node->next, memory_order_relaxed);
	if (!next) {
		qnode *prev = node;
		if (atomic_compare_exchange_strong_explicit(&lock, &prev, NULL,
		    memory_order_release, memory_order_relaxed)) {
			return;
		}

		atomic_thread_fence(memory_order_acquire);

		do {
			next = atomic_load_explicit(&node->next, memory_order_relaxed);
		} while (!next);
	}

	atomic_store_explicit(&next->locked, false, memory_order_release);
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
