# _*_ MakeFile _*_

lab0: lab0.o
	gcc -Wall -Wextra lab0.o -o lab0

lab0.o: lab0.c
	gcc -Wall -Wextra -c lab0.c

check: test1 test2 test3

test1:
	./lab0 --input a.txt

test2:
	./lab0 --segfault

test3:
	./lab0 --segfault --catch
clean: 
	rm -f *.o lab0                                                                                                                                                                                          