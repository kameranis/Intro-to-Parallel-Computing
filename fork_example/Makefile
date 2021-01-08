fork_example : fork_example.o proc-common.o
	@gcc -Wall -Wextra fork_example.o proc-common.o -o fork_example

%.o : %.c
	@gcc -Wall -Wextra -c -o $@ $<

clean :
	-@rm *.o
	-@rm fork_example
