CC = gcc
CFLAGS = -Wextra -Wall -O2 -pthread
all: thread thread_atomic thread_mutex

thread: thread_coordination.c
	$(CC) $(CFLAGS) -o $@ $<

thread_atomic: thread_coordination.c
	$(CC) $(CFLAGS) -o $@ $< -DSYNC_ATOMIC

thread_mutex: thread_coordination.c
	$(CC) $(CFLAGS) -o $@ $< -DSYNC_MUTEX

clean:
	rm thread thread_atomic thread_mutex
