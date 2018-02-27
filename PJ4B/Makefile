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
	echo "TEST with invalid option argument"
	-./lab4b --rua 2> STDERR
	if [ $? -ne 1 ]
	then
		echo "ERROR. Expected RC=1."
	else
		echo "PASS THE TEST."
	rm STDERR
	fi
	echo "TEST with --period"
	echo "OFF" | ./lab4b --period=2 2> STDERR
	if [ $? -ne 0 ]
	then
		echo "ERROR. Expected RC=0."
	else
		echo "PASS THE TEST."
	rm STDERR
	fi
	echo "TEST with --scale"
	echo "OFF" | ./lab4b --scale=C 2> STDERR
	if [ $? -ne 0 ]
	then
		echo "ERROR. Expected RC=0."
	else
		echo "PASS THE TEST."
	rm STDERR
	fi
	echo "Testing --log"
	echo "OFF" | ./lab4b --log="LOGFILE" > /dev/null
	if [ ! -s LOGFILE ]
	then
		echo "ERROR. No log file created."
	else
		echo "PASSES ALL TESTS"
	rm "LOGFILE"
	fi
    


clean: 
	rm -f lab4b *.txt *.tar.gz

dist: 
	@tar -czf lab4b-505029637.tar.gz $(TARGET)