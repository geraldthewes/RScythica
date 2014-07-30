# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.


#' print function for SDataFrame
#'
#' 
#' @param x S Dataframe
#' @export
print.Rcpp_SDataframe <- function(x,...) {
p <- "Scythica Dataframe:"
p <- paste(p,sprintf("Partitions: %d",length(x$partitions)),sep='\n')
p <- paste(p,sprintf("Columns: %d",x$ncol()),sep='\n')
p <- paste(p,paste(x$names(),collapse=' '),sep='\n')
cat(p)
invisible()
} 

#' Extract part of an S Data Frame
#'
#' 
#' @param x S Dataframe
#' @param i Row index of the form partion:split
#' @param j Column
#' @return Vector in that position 
#' #' @examples
#' #v <-  x["2008-01-03:1","AirTime"]
#' @export
"[.Rcpp_SDataframe" <- function(x,i,j) {
  v <- strsplit(i,':')
  p <- v[[1]][1]
  split <- as.numeric(v[[1]][2])
  x$splitn(p,split,j)
}