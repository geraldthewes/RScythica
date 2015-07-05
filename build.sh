#!/bin/sh
DATADIR=tests/extdata
cd $DATADIR
rm -rf airline.sds iris.sds boston.sds noaa.sds
sdscreate airline.yaml airline.sds airline.csv
sdscreate -noheader=true iris.yaml iris.sds iris.data
sdscreate boston.yaml boston.sds boston-1970-2014.csv
sdscreate PRECIP_15_sample_csv.yaml noaa.sds PRECIP_15_sample_csv.csv

