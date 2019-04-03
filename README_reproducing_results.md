# Scaling Tests
All scripts used and intermmediate files generated for these results are located in the folder "scaling tests". For all scripts to work, the parameters in config.py have to be adjusted and python3 has to be installed on the system.

All tests are run through invocing the runTests.py script. This script will take care of automatically generating needed partition and repeats files if they do not exist already. When called with the --help parameter, the following help will be given:
 
	usage: runTests.py [-h] [-p] [-m MACHINE_ID] [-a ALGORITHM] [-d]
		               [-t NTHREADS [NTHREADS ...]] [-s {strong,weak,both}]
		               [-P PARAM] [-c CPU_CONFIG] [-tp {disabled,balanced}]

	Run scaling tests for JudiciousPartitioner.

	optional arguments:
	  -h, --help            show this help message and exit
	  -p, --print-header    Print the CSV header
	  -m MACHINE_ID, --machine-id MACHINE_ID
		                    Machine id to print into the CSV field
	  -a ALGORITHM, --algorithm ALGORITHM
		                    Algorithm name to print into CSV field
	  -d, --dry-run         Do not perform any measurements
	  -t NTHREADS [NTHREADS ...], --nthreads NTHREADS [NTHREADS ...]
		                    List of OMP_THREAD_NUM values to benchmark
	  -s {strong,weak,both}, --scaling {strong,weak,both}
		                    Which kind of scaling test to perform
	  -P PARAM, --param PARAM
		                    Change any values defined in config.py
	  -c CPU_CONFIG, --cpu-config CPU_CONFIG
		                    Defines the CPU configuration as
		                    <nSockets>x<nCoresPerSocket>
	  -tp {disabled,balanced}, --thread_pinning {disabled,balanced}
		                    Choose thread pinning mode. disabled: no thread
		                    pinning. balanced: Use minimum number of sockets for
		                    given number of threads but balance out number of
		                    threads over per socket over the sockets.

We used the folling calls to the runTests.py script to perform our measurements:

exclusive ./runTests.py --algorithm {aligned,sparse} --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K={50,160} --scaling {weak,strong} --param MAX_SITES=160000 -tp balanced --cpu-config {4x8}

Where "exclusive" ensured, that no other job was running on the same machine at the same time and with all other parameters set according to the the paper. A complete list of all calls can be found in the the runSomeStuff.sh in the git. This script was called on each host used in the scaling tests. Beforehand, a directory `hostname`had to be created where the JudiciousCpp and MSAconverter binaries are stored. The compile.sh script has been used to generate such a directory for each host. Be aware, that this script is highly specific to our test setup.

After running all tests on all hosts, the resulting .csv files were concatenated and stored in the results directory. The plots where then generated with the prettyFigures.R script which needs R with the ggplot2, dplyr and readr packages installed.

