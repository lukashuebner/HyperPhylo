#!/bin/bash

regex="^raw/supermatrix/(.*)\.partitions$" 
for filename in raw/supermatrix/*; do
	if [[ $filename =~ $regex ]]; then
		name=${BASH_REMATCH[1]}
		../RepeatsCounter/tools/MSAConverter/build/convert raw/supermatrix/supermatrix_C_nt2.fas $filename raw/supermatrix/supermatrix_C_nt2.newick $name.repeats
	fi
done
	

