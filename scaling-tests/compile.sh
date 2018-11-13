#!/bin/bash

LIST_OF_BRANCHES="SparseBitVector master"
LIST_OF_EXECUTABLE_NAMES=(JudiciousCppSparse JudiciousCppAligned)
LIST_OF_HOSTS="127 128 129 132 134"
JUDICIOUS_SRC_PATH="../JudiciousCppOptimized/"
CMAKE_COMMAND="/software/cmake-3.10.0/bin/cmake .. -DCMAKE_C_COMPILER=/software/clang/6.0.1/bin/clang -DCMAKE_CXX_COMPILER=/software/clang/6.0.1/bin/clang++"

require_clean_work_tree() {
    # Update the index
    git update-index -q --ignore-submodules --refresh
    err=0

    # Disallow unstaged changes in the working tree
    if ! git diff-files --quiet --ignore-submodules --
    then
        echo >&2 "cannot $1: you have unstaged changes."
        git diff-files --name-status -r --ignore-submodules -- >&2
        err=1
    fi

    # Disallow uncommitted changes in the index
    if ! git diff-index --cached --quiet HEAD --ignore-submodules --
    then
        echo >&2 "cannot $1: your index contains uncommitted changes."
        git diff-index --cached --name-status -r --ignore-submodules HEAD -- >&2
        err=1
    fi

    if [ $err = 1 ]
    then
        echo >&2 "Please commit or stash them."
        exit 1
    fi
}

# Check if all paths exist
if [ ! -d "$JUDICIOUS_SRC_PATH" ]; then
    echo "Judicious path does not exist!"
    exit 1
fi

# Check if branches exist
for branch in $LIST_OF_BRANCHES; do
    git rev-parse --verify $branch > /dev/null
    if [ $? -ne 0 ]; then
        echo "Branch $branch does not exist!"
        exit 1
    fi
done

# Check if the repo is in a clean state
require_clean_work_tree compile

# Check if script is running on login node
if [[ `hostname` -ne "i10pc107" ]]; then
	echo >&2 "Must be run on login node."
	exit 1
fi

# For each host compile both branches over ssh
for host_number in $LIST_OF_HOSTS; do
    branch_num=0
    for branch in $LIST_OF_BRANCHES; do
        hostname="i10pc$host_number"
        ssh $hostname "
            cd ~/HyperPhylo/scaling-tests
            git checkout $branch
    		mkdir $hostname
            
            # Copy source and convert executable to the hostname folder
            cp -r $JUDICIOUS_SRC_PATH $hostname/jud_src
        	cp convert $hostname
            		
            # Compile JudiciousCpp
            cd $hostname
            cd jud_src
            mkdir compile
		    cd compile
            $CMAKE_COMMAND
		    make -j 4 JudiciousCpp
            
            # Move the exectuable and rename it
            mv JudiciousCpp ../../${LIST_OF_EXECUTABLE_NAMES[$branch_num]}

            # Clean up
            cd ../..
            rm -r jud_src

        "
        branch_num=$(($branch_num + 1))
        echo "Done with $branch."
	done
done
