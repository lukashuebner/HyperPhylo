#!/usr/bin/env bash

path="../JudiciousPartitioning/cmake-build-debug/JudiciousCpp"

#files='59-s.repeats 128-s.repeats 404-s.repeats 59-l.repeats 128-l.repeats 404-l.repeats'
#files='404-l.repeats'
files='59_single.repeats 404_single.repeats'

corelist='1 2 4 8 16 32 64 128 160'
#corelist=''

rm timings.txt
rm results.txt
for file in ${files}
do
	echo -ne "Running file $file...\t"
	echo "" >> timings.txt
	echo ${file} >> timings.txt
	echo "--------------------------------------" >> timings.txt
	echo "S" >> timings.txt
	echo ${file} >> results.txt
	echo "--------------------------------------" >> results.txt
	echo -n "S "
	{ time ${path}Sequential ../datasets/${file} 2,4,8,12,16,24,32,48,64 >> results.txt ; } 2>> timings.txt
	
	for cores in ${corelist}
	do
		echo -n "$cores "
		echo "Cores:" ${cores} >> timings.txt
		export OMP_NUM_THREADS=${cores}
		{ time ${path}Parallel ../datasets/${file} 2,4,8,12,16,24,32,48,64 >> /dev/null ; } 2>> timings.txt
	done
	echo "Done."
done
