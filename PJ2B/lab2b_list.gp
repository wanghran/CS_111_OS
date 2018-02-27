#! /usr/local/cs/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2b_1.png ... throughput vs Number of Threads for Mutex and Spin-Lock Synchronization.
#	lab2b_2.png ... wait-for-lock time, and the average time per operation against the number of competing threads.
#	lab2b_3.png ... number of successful iterations for each synchronization method.
#	lab2b_4.png ... throughput vs number of threads for mutexes with partitioned lists.
#	lab2b_5.png ... throughput vs number of threads for spin-locks with partitioned lists.

# general plot parameters
set terminal png
set datafile separator ","

# graph1
set title "Lab2B-1: Throughput vs Number of Threads for Mutex and Spin-Lock Synchronization"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Spin-Lock' with linespoints lc rgb 'green'

# graph 2
set title "Lab2B-2: Wait-for-Lock Time and Average Time per Operation"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Wait-for-Lock Time and Average Time per Operation"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'Mutex Wait' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'Completion Time' with linespoints lc rgb 'red'

# graph 3
set title "Lab2B-3: Unprotected and Protected Iterations that Run Without Failure"
set xlabel "Threads"
set logscale x 2
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
    "< grep 'list-id-none' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "red" title "No Synchronization", \
   "< grep 'list-id-m' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "green" title "Mutex", \
   "< grep 'list-id-s' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "blue" title "Spin", \

# graph 4
set title "Lab2B-4: Throughput with Mutex Synchronization"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_4.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'orange'

# graph5
set title "Lab2B-5: Throughput with Spin Lock Synchronization"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_5.png'

plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'orange'