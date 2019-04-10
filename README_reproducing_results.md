# Scaling Tests
All scripts used and intermediate files generated for these results are located in the folder "scaling tests". For all scripts to work, the parameters in config.py have to be adjusted and python3 has to be installed on the system.

All tests are run through invoking the runTests.py script. This script will take care of automatically generating needed partition and repeats files if they do not exist already. When called with the --help parameter, the following help will be given:
 
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

We used the following calls to the runTests.py script to perform our measurements:

exclusive ./runTests.py --algorithm {aligned,sparse} --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K={50,160} --scaling {weak,strong} --param MAX_SITES=160000 -tp balanced --cpu-config {4x8}

Where "exclusive" ensured, that no other job was running on the same machine at the same time and with all other parameters set according to the the paper. A complete list of all calls can be found in the the runSomeStuff.sh in the git. This script was called on each host used in the scaling tests. Beforehand, a directory `hostname`had to be created where the JudiciousCpp and MSAconverter binaries are stored. The compile.sh script has been used to generate such a directory for each host. Be aware, that this script is highly specific to our test setup.

After running all tests on all hosts, the resulting .csv files were concatenated and stored in the results directory. The plots where then generated with the prettyFigures.R script which needs R with the ggplot2, dplyr and readr packages installed.

# Quality Tests
Creating the data for the quality graphs - presenting the performance of the naive approach to partition an MSA partition versus our judicious partitioning - is a multiple step approach:
1. Run the naive and judicious partitioning with `./very-naive-split.py <repeats file> <list of block numbers>` and `./JudiciousPartitioning <repeats file> <list of block numbers>`, respectively for all repeat files that are shown.
2. Perform repeat class counts on all the results to get the metrics by calling `./RepeatsCounter <repeats file> <result file>` where `<repeats file>` needs to be the same that created the result in step 1 and `<result file>` is the said result from step 1.
3. Extract the needed metrics from the RepeatsCounter output and put it in a csv format. For that, we created the script `create_csv_for_quality_tests.py` which expects the used repeats file and the folder with all the RepeatsCounter output (one file per output with specific file name). For specific information about how to call it, there is a --help command which gives information as shown below. Additionally, the name of the used partitioning scheme and the used block numbers needs to be supplied in a constant at the top of the script. The script is run for each repeats file twice, once for the repeat loss metric and once for the WorstRCC metric.
```
usage: create_csv_for_quality_tests.py [-h] [--boxplot] [--loss]
                                       [--print_name]
                                       repeats_file results_folder
                                       repeats_name

Take a bunch of rcccount files and create a csv that contains all the results
in an ordered fashion such that one can create cute graphs from it.

To use this script, all rcccount result files have to be located in one
directory and must comply with the following naming scheme:
  <algorithm>_<repeats_name>_<value_of_k>.rcccount
e.g. for the repeats file 404.repeats, files must be named like this:
  rdda_404_2.rcccount
  rdda_404_4.rcccount
  ...
  hybrid_404_8192.rcccount

positional arguments:
  repeats_file    the repeats file for which you want to check rccc results
  results_folder  the folder where all rcccount result files are located
  repeats_name    the name of the repeats file as in the rcccount file names

optional arguments:
  -h, --help      show this help message and exit
  --boxplot       changes output csv content boxplot requirement
  --loss          Parse repeats loss instead of RCC
  --print_name    Add file name to the output
```
4. The data produced by step 3 gets transposed by calling `./csv_transpose.py < output_step_3` and then concatenated in one big file.
5. Step 1-4 is run 5 times and the results are averaged to reduce influence of parallelization impact by `./average_results.py`. This is imporant as the parallelized judicious partitioning is not deterministic and results in different results each run.

For running the partitioning and running it through RepeatsCounter in one call, we used the script `automation.py`. This script is not very user friendly written, instable and made for our specific test setup, but you can take a look at it to get an idea of the pipeline. For easier csv generation, we also created the script `generateresults.py` which combines metric extraction, transposing and concatenating in one step.
