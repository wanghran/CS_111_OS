#!/bin/bash

# NAME: Haoran Wang
# EMAIL: hwan252@ucla.edu
# ID: 505029637

echo "TEST with invalid option argument"
./lab4b --rua 2> STDERR
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
