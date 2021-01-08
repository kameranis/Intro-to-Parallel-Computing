/* NTUA OS Exercise 2.1
 *
 * The program creates a given process tree
 * 
 * A-+-B--D
 *   `-C
 *
 * Each process runs its own function
 * This approach is not scalable and thus it is discouraged
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

/*
 * Create this process tree:
 * A-+-B---D
 *   `-C
 */

void C (void)
{
    change_pname ("C");
    printf ("C: Sleeping...\n");
    sleep (SLEEP_PROC_SEC);
    printf ("C: Exiting...\n");
    exit (17);
}

void D (void)
{
    change_pname ("D");
    printf ("D: Sleeping...\n");
    sleep (SLEEP_PROC_SEC);
    printf ("D: Exiting...\n");
    exit (13);
}

void B(void)
{
    change_pname ("B");
    int status;
    pid_t p;
    p = fork ();
    if (p < 0) {
        /* fork failed */
        perror ("fork");
        exit (EXIT_FAILURE);
    }
    if (p == 0) {
        /* In child process */
        D ();
    }
    /* print message */
	printf ("Parent, PID = %ld: Created child with PID = %ld...\n",
		(long)getpid (), (long)p);
	p = wait (&status);
	explain_wait_status (p, status);
    printf ("B: Exiting...\n");
    exit (19);
}

void A(void)
{
	/* initial process is A */
	change_pname ("A");
    int status;
    pid_t pB, pC;
    /* Create B */
    pB = fork ();
    if (pB < 0) {
        /* fork failed */
        perror ("fork");
        exit (EXIT_FAILURE);
    }
    if (pB == 0) {
        /* In child process */
        B ();
    }
    /* Create C */
    pC = fork ();
    if (pC < 0) {
        /* fork failed */
        perror ("fork");
        exit (EXIT_FAILURE);
    }
    if (pC == 0) {
        /* In child process */
        C ();
    }
    /* print messages */
	printf ("Parent, PID = %ld: Created child with PID = %ld...\n",
		(long)getpid (), (long)pB);
	printf ("Parent, PID = %ld: Created child with PID = %ld...\n",
		(long)getpid (), (long)pC);

    /* Wait for subprocesses to exit */
	pB = wait (&status);
	explain_wait_status (pB, status);
	pC = wait (&status);
	explain_wait_status (pC, status);
    /* Time to sleep and exit */
	printf ("A: Exiting...\n");
	exit (16);
}

/*
 * The initial process forks the root of the process tree,
 * waits for the process tree to be completely created,
 * then takes a photo of it using show_pstree().
 */
int main(void)
{
	pid_t pid;
	int status;

	/* Fork root of process tree */
	pid = fork ();
	if (pid < 0) {
		perror ("main: fork");
		exit (EXIT_FAILURE);
	}
	if (pid == 0) {
		/* Child */
		A ();
		exit (EXIT_FAILURE);
	}
    
    /* Wait for children to be created */
	sleep (SLEEP_TREE_SEC);

	/* Print the process tree root at pid */
	show_pstree (pid);

	/* Wait for the root of the process tree to terminate */
	pid = wait (&status);
	explain_wait_status (pid, status);

	exit (EXIT_SUCCESS);
}
