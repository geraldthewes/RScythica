# RScythica

RScythica Dataframes, called S Dataframes, are large scale,read-only,
eventually distributed, data frames which are both partioned and
broken into large splits. S Dataframes are created using external utilities.

Major functionality include:

* S Dataframes consists of multiple binary files that hold vector daya

* S Dataframes are partioned. 

* Partitions are broken into large splits, typically 64MB in size

Dependencies

Linux

Mac OS X

  brew install boost
  brew install yaml-cpp
  brew install msgpack

Test Data

sdscreate airline.yaml airline.db airline.csv
sdscreate -noheader=true iris.yaml iris.db iris.data