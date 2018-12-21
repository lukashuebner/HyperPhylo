#!/bin/bash

REPEATS_FILE="../scaling-tests/repeats-files/supermatrix_subsample_single_partiton_50000.repeats"
# REPEATS_FILE="../datasets/extracted/59-l.repeats"
EXECUTABLE="./JudiciousPartitioning$1"
OUTPUT_FILE="numa_result$1.txt"
k=50
# export KMP_AFFINITY=verbose
start_time=$SECONDS
total=0
runtime_string=""

function time_string {
	local result
	local hours
	local minutes
	local seconds

	hours=$(( $1 / 3600 ))
	minutes=$(( ($1 / 60) % 60 ))
	seconds=$(( $1 % 60 ))

	if [ $hours -lt 10 ]; then
		result=$result"0"
	fi
	result=$result$hours":"

	if [ $minutes -lt 10 ]; then
                result=$result"0"
        fi
        result=$result$minutes":"

	if [ $seconds -lt 10 ]; then
                result=$result"0"
        fi
        result=$result$seconds
	echo $result
}

function runtime {
	local duration
	duration=$(( $SECONDS - $start_time ))
	total=$(( $duration + $total ))
	runtime_string=$(time_string $duration)" ("$(time_string $total)" total)"
	start_time=$SECONDS
}

echo "# Reproduction test - no pinning 4 threads" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=4 $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE;
echo "# Reproduction test - no pinning 12 threads" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=12 $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE;
echo "# Reproduction test - no pinning 16 threads" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=16 $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE;
echo "# Reproduction test - no pinning 17 threads" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=17 $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE;
echo "# Reproduction test - no pinning 18 threads" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=18 $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE;
echo "# Reproduction test - no pinning 32 threads" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=32 $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE;

runtime
./notifier.py "\[$(hostname)] No Pinning block finished after ""$runtime_string"

echo "# Pinning lowest NUMA node amount possible - 4 threads (1 node)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="0-3" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible - 12 threads (2 nodes)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=12 GOMP_CPU_AFFINITY="0-11" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible - 16 threads (2 nodes)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=16 GOMP_CPU_AFFINITY="0-15" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible - 17 threads (3 nodes)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=17 GOMP_CPU_AFFINITY="0-16" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible - 18 threads (3 nodes)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=18 GOMP_CPU_AFFINITY="0-17" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible - 32 threads (4 nodes)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=32 GOMP_CPU_AFFINITY="0-31" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE

runtime
./notifier.py "\[$(hostname)] Ascending binding block finished after ""$runtime_string"

echo "# Pinning lowest NUMA node amount possible but all nodes get the same amount of threads - 4 threads (1 node)" | tee -a $OUTPUT_FILE;
echo "Runtime: check 'lowest amount possible - 4 threads'" | tee -a $OUTPUT_FILE;
echo "# Pinning lowest NUMA node amount possible but all nodes get the same amount of threads - 12 threads (2 nodes, 6 threads per)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=12 GOMP_CPU_AFFINITY="0-5,8-13" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible but all nodes get the same amount of threads - 16 threads (2 nodes, 8 threads per)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=16 GOMP_CPU_AFFINITY="0-7,8-15" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible but all nodes get the same amount of threads - 17 threads (3 nodes, 5/6/6 threads)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=17 GOMP_CPU_AFFINITY="0-4,8-13,16-21" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible but all nodes get the same amount of threads - 18 threads (3 nodes, 6 threads per)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=18 GOMP_CPU_AFFINITY="0-5,8-13,16-21" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning lowest NUMA node amount possible but all nodes get the same amount of threads - 32 threads (4 nodes, 8 threads per)" | tee -a $OUTPUT_FILE;
echo "Runtime: check 'lowest amount possible - 32 threads'" | tee -a $OUTPUT_FILE;

runtime
./notifier.py "\[$(hostname)] Balanced across lowest binding block finished after ""$runtime_string"

echo "# Pinning to all NUMA nodes equally - 4 threads (1 thread per)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=4 GOMP_CPU_AFFINITY="0,8,16,24" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning to all NUMA nodes equally - 12 threads (3 threads per)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=12 GOMP_CPU_AFFINITY="0-2,8-10,16-18,24-26" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning to all NUMA nodes equally - 16 threads (4 threads per)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=16 GOMP_CPU_AFFINITY="0-3,8-11,16-19,24-27" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning to all NUMA nodes equally - 17 threads (4/4/4/5 threads)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=17 GOMP_CPU_AFFINITY="0-3,8-11,16-19,24-28" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning to all NUMA nodes equally - 18 threads (4/4/5/5 threads)" | tee -a $OUTPUT_FILE;
OMP_NUM_THREADS=18 GOMP_CPU_AFFINITY="0-3,8-11,16-20,24-28" $EXECUTABLE $REPEATS_FILE $k >> $OUTPUT_FILE
echo "# Pinning to all NUMA nodes equally - 32 threads (8 threads per)" | tee -a $OUTPUT_FILE;
echo "Runtime: check 'lowest amount possible - 32 threads'" | tee -a $OUTPUT_FILE;

runtime
./notifier.py "\[$(hostname)] Balanced binding block finished after ""$runtime_string"
./notifier.py "\[$(hostname)] Run finished!"
