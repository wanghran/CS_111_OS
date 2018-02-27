#Name: Haoran Wang
#ID: 505029637
#Email: hwan252@ucla.edu
#_*_MakeFile _*_

CC = gcc
SET = -Wall -Wextra -g
TARGET = lab3a.c Makefile README ext2_fs.h

default: build

build: lab3a 

lab3a: lab3a.c
	$(CC) $(SET) lab3a.c -o lab3a

clean: 
	rm -f lab3a *.csv *.tar.gz

dist: 
	@tar -czf lab3a-505029637.tar.gz $(TARGET)