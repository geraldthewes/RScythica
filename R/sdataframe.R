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
