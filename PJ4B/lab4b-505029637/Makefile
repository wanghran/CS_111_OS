#Name: Haoran Wang
#ID: 505029637
#Email: hwan252@ucla.edu
#_*_MakeFile _*_

CC = gcc
SET = -Wall -Wextra -lmraa -g -lpthread -lm
TARGET = lab4b.c Makefile README lab4b_check.sh

default: build

build: lab4b

lab4b: lab4b.c
		$(CC) $(SET) lab4b.c -o lab4b

check: build
	chmod +x lab4b_check.sh
	./lab4b_check.sh
    


clean: 
	rm -f lab4b *.txt *.tar.gz

dist: 
	@tar -czf lab4b-505029637.tar.gz $(TARGET)