#Name: Haoran Wang
#ID: 505029637
#Email: hwan252@ucla.edu
#_*_MakeFile _*_

lab1a: lab1a.c
	gcc -Wall -Wextra -g lab1a.c -o lab1a

clean:
	rm -f *.tar.gz lab1a

dist: lab1a.c Makefile README
	tar -czf lab1a-505029637.tar.gz lab1a.c Makefile README
