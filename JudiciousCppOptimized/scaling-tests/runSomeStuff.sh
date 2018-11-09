#!/bin/sh

DRY_RUN="--dry-run"

cd HyperPhylo/JudiciousCppOptimized/scaling-tests/

# $ALGORITHM
case `hostname` in
i10pc134)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong --param THREAD_PINNING=True $DRY_RUN | tee `hostname`-testResults.csv

        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=160 --scaling weak --param THREAD_PINNING=True $DRY_RUN | tee --append `hostname`-testResults.csv

        exclusive ./runTests.py --algorithm sparse --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong --param THREAD_PINNING=True $DRY_RUN | tee --append `hostname`-testResults.csv
        ;;
i10pc127|i10pc132)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong --param THREAD_PINNING=True $DRY_RUN | tee `hostname`-testResults.csv

        exclusive ./runTests.py --algorithm sparse --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong --param THREAD_PINNING=True $DRY_RUN | tee --append `hostname`-testResults.csv
        ;;
i10pc129)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=50 --scaling strong --param THREAD_PINNING=True $DRY_RUN | tee `hostname`-testResults.csv
    ;;
i10pc128)
        exclusive ./runTests.py --algorithm aligned --machine-id `hostname` --nthreads 1 2 4 8 9 10 12 16 17 18 20 24 25 26 28 32 --param MSACONVERTER_BIN=`hostname` --param JUDICIOUS_BIN=`hostname` --param K=160 --scaling weak --param THREAD_PINNING=True $DRY_RUN | tee `hostname`-testResults.csv
        ;;
esac

