#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#define BUG 1

struct node {
	pthread_mutex_t mutex;
	struct node *next;
	int value;
};

struct node head = {
	.mutex = PTHREAD_MUTEX_INITIALIZER,
	.next = NULL,
	.value = 0,
};

void insert(int value) {
	struct node *node = &head;
	struct node *next;

	pthread_mutex_lock(&node->mutex);
	while ((next = node->next)) {
#if BUG
		pthread_mutex_unlock(&node->mutex);
		pthread_mutex_lock(&next->mutex);
		if (next->value < value) {
			node = next;
		} else {
			pthread_mutex_unlock(&next->mutex);
			pthread_mutex_lock(&node->mutex);
			break;
		}
#else
		pthread_mutex_lock(&next->mutex);
		if (next->value < value) {
			pthread_mutex_unlock(&node->mutex);
			node = next;
		} else {
			pthread_mutex_unlock(&next->mutex);
			break;
		}
#endif
	}

	next = malloc(sizeof(*next));
	pthread_mutex_init(&next->mutex, NULL);
	next->value = value;
	next->next = node->next;
	node->next = next;
	pthread_mutex_unlock(&node->mutex);
}

void *func(void *ptr) {
	insert(6);
	insert(4);
	insert(2);
	return NULL;
}

int main(void) {
	pthread_t thread;
	pthread_create(&thread, NULL, func, NULL);
	insert(1);
	insert(3);
	insert(5);
	pthread_join(thread, NULL);

	for (struct node *node = &head; node; node = node->next) {
		printf("%d", node->value);
		printf(node->next ? " -> " : "\n");
	}

	return 0;
}
