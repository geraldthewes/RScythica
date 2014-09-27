# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# Note: Inspired from the bit package: http://cran.r-project.org/package=bit

#' Create a bit file 
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

bit <- function(length){
    length <- as.integer(length)
    if (length %% .BITS)
      n <- length %/% .BITS + 1L
    else
      n <- length %/% .BITS
    x <- integer(n)


    attributes(x) <- list(class="bit")
    x
}
