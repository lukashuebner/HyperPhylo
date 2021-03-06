#!/bin/sh

#DRY_RUN="--dry-run"
DRY_RUN=""

cd HyperPhylo/scaling-tests/

# $ALGORITHM
case `hostname` in
i10pc134)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong -tp balanced --cpu-config 4x8 $DRY_RUN | tee `hostname`-testResults.csv

        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=160 --scaling weak --param MAX_SITES=160000 -tp balanced --cpu-config 4x8  $DRY_RUN | tee --append `hostname`-testResults.csv

        exclusive ./runTests.py --algorithm sparse --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong -tp balanced --cpu-config 4x8  $DRY_RUN | tee --append `hostname`-testResults.csv
        ;;
i10pc127)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong -tp balanced --cpu-config 4x8 $DRY_RUN | tee `hostname`-testResults.csv

        exclusive ./runTests.py --algorithm sparse --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong -tp balanced --cpu-config 4x8 $DRY_RUN | tee --append `hostname`-testResults.csv
        ;;
i10pc128)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=160 --scaling weak --param MAX_SITES=160000 -tp balanced --cpu-config 4x8 $DRY_RUN | tee `hostname`-testResults.csv
	;;
i10pc129)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong -tp balanced --cpu-config 2x8 $DRY_RUN | tee `hostname`-testResults.csv
	;;
i10pc132)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong -tp balanced --cpu-config 2x16 $DRY_RUN | tee `hostname`-testResults.csv

        exclusive ./runTests.py --algorithm sparse --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong -tp balanced --cpu-config 2x16 $DRY_RUN | tee --append `hostname`-testResults.csv
        ;;
esac
