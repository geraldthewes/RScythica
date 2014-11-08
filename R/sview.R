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
          bitmap=NULL,
          klen=NULL,
          rows=0)
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
  rows <- 0
  for (i in 1:nrow(v$partitions)) {
    splits <- (v$partitions)[i,"splits"]
    p <- as.character((v$partition)[i,"partition"])
    for (s in 1:splits) {    
      srows <-  if (s == splits) (v$partitions)[i,"remainder"] else rows_per_split
      sindex <- sindex(srows)
      vfactory <- m$SIntVector
      bm.v <- new(vfactory)
      b <- bm.v$op.gt((v$ds)$splitn(p,s,(v$filter)$var) ,
                      sindex,
                      (v$filter)$val)
      k <- sdataframe_key(p,s)
      v$bitmap[[k]] <- b
      bv <- new(bm,b)
      v$klen[[k]] <-  bv$popcount()
      rows <- rows + v$klen[[k]]
    }
  }
  v$rows <- rows
  v
}

#' Return number of rows in filter
#'
#' @param v Scythica View
#' @return number of rows after filtering
#' @examples
#'  frow <- sview_rows(views)
#' @export 
sview_rows <- function(v) {
  v$rows
}

#' Execute query
#'
#' @param v Scythica View
#' @return materialized dataframe
#' @examples
#'  view <- sview(views, "2008-01-03")
#' @export 
sview_execute <- function(v) {
  types <- (v$ds)$col_types()
  res <- NULL
  
  for (c in v$columns) {
    col <- switch(types[c],
             int32=integer(v$rows),
             factor=factor(v$rows),
             logical=logical(v$rows),
             integer(v$rows))
   
    if (is.null(res)) {
      res <- data.frame(col)
      names(res)[1] = c
    } else {
      res <- cbind(res,col)   
      names(res)[-1] = c
    }
  }
  
  rows = 1L;
  #rows_per_split <- (v$ds)$rows_per_split
  m   <- Module("rscythica", PACKAGE="RScythica")
  integer.vec <- new(m$SIntVector)
  for (i in 1:nrow(v$partitions)) {
    splits <- (v$partitions)[i,"splits"]
    p <- as.character((v$partition)[i,"partition"])
    for (s in 1:splits) {  
      k <- sdataframe_key(p,s)
      nrows  <- rows + v$klen[[k]] - 1L
      for (c in v$columns) {
        col <- switch(types[c],
                      int32=integer.vec$collapse((v$ds)$splitn(p,s,c),(v$bitmap)[[k]]),
                      factor=NULL,
                      logical=NULL,
                      NULL)      
        res[[c]][rows:nrows] <- col 
      }
      rows <- nrows + 1L
    }
  }
  res
}


## Helper funtions for filters

#' @export 
sfilter <- function(.v, ...) {
  .dos = lazyeval::lazy_dots(...)
} 


#' @export 
parse_filter <- function(ds, condition) {
  parse_filter_(ds, condition[[1]]$expr, condition[[1]]$env)
}

parse_filter_ <- function(ds, expr, env) {
  parsed <- expr
  if (is.call(expr)) {
    op = expr[[1]]
    #print(paste('call:op',op,sep='='))
    parsed <- switch(as.character(op),
                     '&'= parse_logical(ds,expr,env),
                     '|'= parse_logical(ds,expr,env),
                     '>'= parse_operator(ds,expr,env),
                     '<'= parse_operator(ds,expr,env),
                     '=-'= parse_operator(ds,expr,env),
                     '<='= parse_operator(ds,expr,env),
                     '>='= parse_operator(ds,expr,env),
                     stop("Unsupported operator ", op, call. = FALSE))
  } else if (is.name(expr)) {
    #print(paste('name',expr,sep=':'))
    if (!(as.character(expr) %in% ds$names())) {
      parsed <- interp(expr, .values=env)
    } 
  } else if (is.atomic(expr)) {
    ##print(paste('atomic',expr,sep=':'))
  } else {
    stop("Unexpected type ", typeof(expr), call. = FALSE)
  }
  parsed
}

parse_logical <- function(ds,expr,env) {
  parsed <- expr
  for(i in 2:length(expr)) {
    #print(paste('call arguments',expr[[i]],typeof(expr[[i]]),class(expr[[i]]),sep=':'))
    parsed[[i]] <- parse_filter_(ds,expr[[i]],env)
  }
  parsed
}


parse_operator <- function(ds,expr,env) {
  parsed <- expr
  if(length(expr) != 3) {
    stop(expr[[1]], " requires two arguments ", expr, call. = FALSE)
  }
  left <- parse_filter_(ds,expr[[2]],env)
  right <- parse_filter_(ds,expr[[3]],env)
  
  if (is.name(right)) {
    parsed[[1]] <- switch(as.character(expr[[1]]),
                          '>' = as.name('<'),
                          '>=' = as.name('<='),
                          '<' = as.name('>'),
                          '<=' = as.name('>='),
                          expr[[1]])
    parsed[[2]] <- right
    parsed[[3]] <- left
  } else {
    parsed[[2]] <- left
    parsed[[3]] <- right
  }
  parsed
}

