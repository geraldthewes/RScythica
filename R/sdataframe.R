# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

s.factory <- new.env(parent = emptyenv())
s.factory$m   <- Module("rscythica", PACKAGE="RScythica")

#' Create an initial configuration file 
#'
#' This function is used to generate a sample configuration from a sample of the data. 
#' The configuration file can then be edited further. The sample of the data is obtained by
#' reading a sample of the dataset using a function like read.table or read.csv. Since
#' S datasets are aimed are very large datasets, you usually only need to read a few records,
#' enough for the import type to auto detect column types.
#'
#' @param ofile file name path of configuration file to create
#' @param df dataframe to use as example for file format
#' @param rows.per.split Rows per split, usually a very large number
#' @param is.na String representing NA in the data
#' @examples
#' \dontrun{
#'  sample <- read.table('tests/extdata/airline.csv',sep=',', header=TRUE, nrows=100)
#'  makeconfig('example.yaml',sample)
#' }
#' @export
makeconfig <- function(ofile,df,rows.per.split=1000000,is.na='NA') {
  
  columns <- list()
  for (i in names(df)) {
    col <- df[[i]]
    c <- class(col)
    l <- list(list(colname = i, coltype=c))
    columns <- c(columns,l)
  }
  
  
  keyspace=list(key_size=8192,nodes=1,rows_per_split=rows.per.split,isna=is.na)
  cfg <- list(columns=columns,keyspace=keyspace)
  
  cfg.yaml <- as.yaml(cfg)
  write(cfg.yaml,file=ofile)
}


#' Open an S data set file
#'
#' @param path file name path to the s dataset
#' @return SDataframe 
#' @examples
#' \dontrun{
#'  sdf <- open_sdataset('rscythica/tests/extdata/airline.sds')
#' }
#' @export
open_sdataset <- function(path) {
  sdf <- s.factory$m$SDataframe
  sds <- new(sdf,path=path)
}


#' print function for SDataFrame
#'
#' @param x S Dataframe
#' @export
print.Rcpp_SDataframe <- function(x,...) {
p <- "Scythica Dataframe:"
p <- paste(p,
           sprintf("Partitions: %d KeySeparator %s Rows Per Split: %d",
                   length(x$partitions()),
                   x$key_separator,
                   x$rows_per_split),
           sep='\n')
p <- paste(p,sprintf("Columns: %d",x$ncol()),sep='\n')
p <- paste(p,paste(x$names(),collapse=' '),sep='\n')
cat(p)
invisible()
} 

#' Extract part of an S Data Frame
#'
#' @param x S Dataframe
#' @param i Row index of the form partion:split
#' @param j Column
#' @return Vector in that position 
#' @examples
#' \dontrun{
#' v <-  x["2008-01-03:1","AirTime"]
#' }
#' @export
"[.Rcpp_SDataframe" <- function(x,i,j) {
  v <- strsplit(i,':')
  p <- v[[1]][1]
  split <- as.numeric(v[[1]][2])
  x$splitn(p,split,j)
}


#' Return a split as a dataframe
#'
#' Access a split as a dataframe
#'
#' @param sds S Dataframe
#' @param partition key
#' @param split index
#' @return Returns the split as a dataframe
#' @examples
#' \dontrun{
#'  df <- as_data_frame_split(airline,'2008-01-03',1)
#' }
#' @export
as_data_frame_split <- function (sds,partition,split=1) {
  df <- NULL
  col.names <- sds$names()
  o <- 1
  for ( i in 1:sds$ncol()) {
    v <- sds$split(partition, split, i)
    if (!is.null(v)) {
      if (is.null(df)) {
        df <- data.frame(v)
      } else {
        df <- cbind(df, v)
      }
      names(df)[o] <- col.names[i]
      o <- o+1
    }
  }
  df
}

#' Create unique key for S dataframe and split
#'
#' @param partition key
#' @param split index
#' @return Key
#' @examples
#'  k- sdataframe_key'2008-01-03',1)
#' @export
sdataframe_key <- function(partition, split) {
  paste(partition,as.character(split),sep=":")
}
