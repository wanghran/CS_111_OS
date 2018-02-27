#Name: Haoran Wang
#ID: 505029637
#Email: hwan252@ucla.edu
# _*_ MakeFile _*_

all: lab1b-client lab1b-server

lab1b-client: lab1b-client.c
	gcc --save-temps -Wall -Wextra -g -lmcrypt lab1b-client.c -o lab1b-client

lab1b-server: lab1b-server.c
	gcc --save-temps -Wall -Wextra -g -lmcrypt lab1b-server.c -o lab1b-server

clean:
	rm -f *.tar.gz lab1b-server lab1b-client *.txt *.i *.o *.s

dist: lab1b-client.c lab1b-server.c README Makefile my.key
	tar -czf lab1b-505029637.tar.gz lab1b-client.c lab1b-server.c Makefile README my.key
