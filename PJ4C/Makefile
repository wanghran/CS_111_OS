#Name: Haoran Wang
#ID: 505029637
#Email: hwan252@ucla.edu
#_*_MakeFile _*_

CC = gcc
SET = -Wall -Wextra -lmraa -lssl -lcrypto -lm -lpthread
TARGET = lab4c_tcp.c lab4c_tls.c Makefile README

default: lab4c_tcp lab4c_tls

lab4c_tcp: lab4c_tcp.c 
	$(CC) $(SET) lab4c_tcp.c -o lab4c_tcp

lab4c_tls: lab4c_tls.c
	$(CC) $(SET) lab4c_tls.c -o lab4c_tls

clean:
	rm -f lab4c_tls lab4c_tcp *.tar.gz *.txt

dist:
	@tar -czf lab4c-505029637.tar.gz $(TARGET)