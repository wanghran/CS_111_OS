#Name: Haoran Wang
#ID: 505029637
#Email: hwan252@ucla.edu
#_*_MakeFile _*_

CC = gcc
SET = -Wall -Wextra -g -lpthread
TARGET = lab2_list.c SortedList.c SortedList.h Makefile lab2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png README lab2b_list.gp profile.out

default: build

build: lab2_list

lab2_list: lab2_list.c SortedList.c SortedList.h
	$(CC) $(SET) -o lab2_list lab2_list.c SortedList.c

tests:
	#graph 1 graph2
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv

	#graph3
	-./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=10 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --sync=m --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=10 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --sync=s --lists=4 --yield=id >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --sync=s --lists=4 --yield=id >> lab2b_list.csv

	#graph4
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=1 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=1 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=1 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=1 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=1 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv

	#graph5
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=1 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=1 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=1 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=1 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=1 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv

profile: lab2_list
		-rm -f ./raw.gprof
		export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`echo /u/cs/grad/haoranwa/Documents/CS111/PJ2B/`lib
		$(CC) $(SET) -lprofiler lab2_list.c SortedList.c -o lab2_list
		CPUPROFILE=raw.gprof ./lab2_list --threads=12 --iterations=1000 --sync=s 
		pprof --text lab2_list ./raw.gprof > profile.out
		pprof --list=threads_opt lab2_list ./raw.gprof >> profile.out
		rm -f ./raw.gperf

graphs:
	gnuplot lab2b_list.gp

clean: 
	rm -f lab2_list *.tar.gz *.gprof

dist: default profile tests graphs
	@tar -czf lab2b-505029637.tar.gz $(TARGET)

