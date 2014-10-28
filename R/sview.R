# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.


#' Create an sview
#'
#' @param ds Scythica Dataframe
#' @return view 
#' @examples
#'  view <- sview(ds)
#' @export 
sview <- function(ds) {
v  = list(ds=ds,
          partitions=NULL, 
          columns=NULL,
          filter=NULL,
          bitmap=NULL)
class(v) <- "sview"
v
}


#' Print
#'
#' @param v Scythica view 
#' @examples
#'  print(view)
#' @export 
sview.print <- function(v) {
  print(ds)
}


#' Add column filter
#'
#' @param v Scythica View
#' @param columns List of column names
#' @return view 
#' @examples
#'  view <- sview(views, c("DepTime","ArrTime"))
#' @export 
sview_columns <- function(v, columns) {
  cols <- (v$ds)$names()
  for ( c in columns) {
    if (!(c %in% cols)) {
      warning(paste(c,"not in sdataframe columns - ignoring filter",sep=' '))
      return(v)
    }
  }
  v$columns <- columns
  v 
}

#' Add partition filter by range
#'
#' @param v Scythica View
#' @param from Starting partition
#' @param to End partition
#' @return view 
#' @examples
#'  v <- sview_partitions_range(v,c("2008-01-01"),c('2008-01-10'))
#' @export 
sview_partitions_range <- function(v, from, to) {
  v$partitions <- (v$ds)$partitions_range(from,to)
  v 
}

#' Add partition filter by regex
#'
#' @param v Scythica View
#' @param exp Regular Expression
#' @return view 
#' @examples
#'  v <- sview_partitions_regex(v,c("2008-01-0?"))
#' @export 
sview_partitions_regex <- function(v, from, to) {
  v$partitions <- (v$ds)$partitions_regex(from,to)
  v 
}


#' Filter based on condition
#'
#' @param v Scythica View
#' @return number of rows
#' @examples
#'  v <- sview_filter(v, "ArrTime", "eq", 24)
#' @export 
sview_filter <- function(v, variable, operation, value) {
   v$filter <- list(var=variable,op=operation,val=value)
   v
}


#' Return number of raw rows in filter
#'
#' @param v Scythica View
#' @return number of raw rows before filtering
#' @examples
#'  frow <- sview_rawrows(views)
#' @export 
sview_rawrows <- function(v) {
  rows = 0;

  if (!is.null(v$partitions)) {
    rows <- sum((v$partitions)$rows)
  }
  
  rows
}


#' Create bitmap filters
#'
#' @param v Scythica View
#' @return v
#' @examples
#'  frow <- sview_execute_filter(views)
#' @export 
sview_execute_filter <- function(v) {
  rows = sview_rawrows(v)

  if (is.null(v$filter)) {
    v$bitmap <- NULL
    return(v)
  }

  rows_per_split <- (v$ds)$rows_per_split
  m   <- Module("rscythica", PACKAGE="RScythica")
  bm <- m$BitVector
  v$bitmap <- list()
  for (i in 1:nrow(v$partitions)) {
    splits <- (v$partitions)[i,"splits"]
    p <- as.character((v$partition)[i,"partition"])
    for (s in 1:splits) {    
      srows <-  if (s == splits) (v$partitions)[i,"remainder"] else rows_per_split
      sindex <- sindex(srows)
      vfactory <- m$SIntVector
      bm.v <- new(vfactory)
      a <- bm.v$op.gt((v$ds)$splitn(p,s,(v$filter)$var) ,
                      sindex,
                      (v$filter)$val)
      k <- sdataframe_key(p,s)
      v$bitmap[[k]] <- a
    }
  }
  v
}


#' Execute query
#'
#' @param v Scythica View
#' @return materialized dataframe
#' @examples
#'  view <- sview(views, "2008-01-03")
#' @export 
sview_execute <- function(v) {
  rows = sview_rawrows(v)
  types <- (v$ds)$col_types()
  res <- NULL
  
  for (c in v$columns) {
    col <- switch(types[c],
             int32=integer(rows),
             factor=factor(rows),
             logical=logical(rows),
             integer(rows))
   
    if (is.null(res)) {
      res <- data.frame(col)
      names(res)[1] = c
    } else {
      res <- cbind(res,col)   
      names(res)[-1] = c
    }
  }
  
  rows = 1L;
  rows_per_split <- (v$ds)$rows_per_split
  for (p in v$partitions) {
    splits <- (v$ds)$partition_splits(p)
    for (s in 1:splits) {  
      srows <-  if (s == splits) (v$ds)$partition_rows(p) %% rows_per_split else rows_per_split
      nrows  <- rows + srows - 1L
      for (c in v$columns) {
        res[[c]][rows:nrows] <- (v$ds)$splitn(p,s,c) 
      }
      rows <- nrows + 1L
    }
  }
  res
}