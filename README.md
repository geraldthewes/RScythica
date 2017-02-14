# RScythica

RScythica Dataframes, also called S Dataframes, are large scale, read-only, memory-mapped
eventually distributed, data frames which are both partioned and
broken into large splits. S Dataframes are created using external utilities.

LICENSE: LGPL 2.1

Note: RScythica is under development and is not production ready. Interfaces are evolving and subject to change.

Note2: Some functions currently require C++ support for SSE 4.1 instructions.

Features
========

* S Dataframes consists of multiple binary files that hold vector data, that can be mapped directly into an R process

* S Dataframes are partioned. 

* Partitions are further broken into large splits for efficiency

For more defails, check the R Package documentation.

Dependencies
============

Common
------

RScythica depends on the [scythica](http://github.com/geraldthewes/scythica) utilities to create the S datasets, and the utilities  need to be available in the executable search path.

Linux (Ubuntu)
---------------

`sudo apt-get install libyaml-cpp-dev`

`sudo apt-get install libmsgpack-dev`

`sudo apt-get install libboost-regex-dev`

`sudo apt-get install libboost-iostreams-dev`

`sudo apt-get install libboost-system-dev`

`sudo apt-get install libboost-filesystem-dev`

`sudo apt-get install texlive-full`

`sudo apt-get install libxml2-dev`



Mac OS X
--------

The following packages are required


* XCode

* Brew Packages:
 
`brew install boost`

`brew install yaml-cpp`

`brew install msgpack`

Install
-------

library(devtools)
devtools::install_github("geraldthewes/RScythica")


Test Data
=========

The package include source data that needs to be converted to binary form using sdscreate in the `tests/extdata` directory

`sdscreate airline.yaml airline.sds airline.csv`

`sdscreate -noheader=true iris.yaml iris.sds iris.data`

`sdscreate boston.yaml boston.sds boston-1970-2014.csv`

`sdscreate PRECIP_15_sample_csv.yaml noaa.sds PRECIP_15_sample_csv.csv`

Build
=====

The Doxygen file for the C++ code can be created with

`doxygen`

`cd latex`

`make`

This will create an HTML version in the HTML/index.html subdirectory, and a PDF version in the latex subdirectory.

Limitations
===========

Current limitations include:

* Limited testing or error handling
* Only supports POSIX systems such as MacOS X and Linux
* Currently supports the following data types:
    * integer (32-bit)
    * numeric (64-bit Double)
    * factor
    * logicals
    * Date
    * DateTime (POSIXct)
* Currently Requires SSE 4.1 Support to accelerate vector operations.