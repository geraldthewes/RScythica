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
          parsed=NULL,
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


#' Select columns
#'
#' @param v Scythica View
#' @param columns List of column names
#' @return view 
#' @examples
#'  view <- sview(views, c("DepTime","ArrTime"))
#' @export 
sv_subset <- function(v, columns) {
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
#' @param l list of partitions
#' @return view 
#' @examples
#'  v <- sview_partitions(v,c("2008-01-03"))
#' @export 
sv_partitions <- function(v, l) {
  v$partitions <-  (v$ds)$partitions_list(l)
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
sv_partitions_range <- function(v, from, to) {
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
sv_partitions_regex <- function(v, from, to) {
  v$partitions <- (v$ds)$partitions_regex(from,to)
  v 
}


#' Filter based on condition
#'
#' @param v Scythica View
#' @param filter expressions
#' @return view
#' @examples
#'  v <- sview_filter(v, Distance > 1000 & TaxiIn < 20)
#' @export 
sv_filter <- function(v, ...) {
   v$filter <- lazyeval::lazy_dots(...)
   v <- sv_execute_filter(v)
}


#' Return number of raw rows in filter
#'
#' @param v Scythica View
#' @return number of raw rows before filtering
#' @examples
#'  frow <- sview_rawrows(views)
#' @export 
sv_rawrows <- function(v) {
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
sv_execute_filter <- function(v) {
  # Parse filter
  if (is.null((v$ds)$names())) {
    stop("Applying filter on intialized view")
  }
  columns <- (v$ds)$names()
  parsed <- parse_filter(columns, v$filter) 
  v$parsed <- parsed
  
  # Extract the columns from the expression
  cols <- unique(unlist(columns_from_filter(parsed,list())))
  
  
  # iterate over all the partitions and slits
  v$bitmap <- list()
  rows <- 0
  for (i in 1:nrow(v$partitions)) {
    splits <- (v$partitions)[i,"splits"]
    p <- as.character((v$partition)[i,"partition"])
    for (s in 1:splits) {  
      # Map environment
      for (col in cols) {
        vec <- (v$ds)$splitn(p,s,col)
        assign(col, vec)
      }
      # Evaluate index
      b <- eval(parsed)
      k <- sdataframe_key(p,s)
      v$bitmap[[k]] <- b
      v$klen[[k]] <-  sindex_popcount(b)
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
sv_rows <- function(v) {
  v$rows
}

#' Execute query
#'
#' @param v Scythica View
#' @return materialized dataframe
#' @examples
#'  view <- sview(views, "2008-01-03")
#' @export 
sv_execute <- function(v) {
  types <- (v$ds)$col_types()
  res <- NULL
  
  if (is.null(v$columns)) {
    columns <- (v$ds)$names()
  } else {
    columns <- v$columns()
  }
  
  for (c in columns) {
    col <- switch(types[c],
             int32=integer(v$rows),
             double=numeric(v$rows),
             factor={ f<-integer(v$row); attr(f,'levels')<-(v$ds)$col_levels(c); class(f)<-'factor';f},
             logical=logical(v$rows),
             integer(v$rows))
   
    if (is.null(res)) {
      res <- data.frame(col)
      names(res)[1] = c
    } else {
      res <- cbind(res,col)   
      names(res)[ncol(res)] = c
    }
  }
  
  rows = 1L;
  #rows_per_split <- (v$ds)$rows_per_split
  m   <- Module("rscythica", PACKAGE="RScythica")
  integer.vec <- new(m$SIntVector)
  numeric.vec <- new(m$SNumericVector)
  factor.vec <- new(m$SFactorVector)
  for (i in 1:nrow(v$partitions)) {
    splits <- (v$partitions)[i,"splits"]
    p <- as.character((v$partition)[i,"partition"])
    for (s in 1:splits) {  
      k <- sdataframe_key(p,s)
      nrows  <- rows + v$klen[[k]] - 1L
      for (c in columns) {
        col <- switch(types[c],
                      int32=integer.vec$collapse((v$ds)$splitn(p,s,c),(v$bitmap)[[k]]),
                      numeric=numeric.vec$collapse((v$ds)$splitn(p,s,c),(v$bitmap)[[k]]),
                      factor=factor.vec$collapse((v$ds)$splitn(p,s,c),(v$bitmap)[[k]]),
                      logical=NULL,
                      NULL)   
        if (!is.null(col)) {
          res[[c]][rows:nrows] <- col
        }
      }
      rows <- nrows + 1L
    }
  }
  res
}


## Helper funtions for filters

#' @export 
parse_filter <- function(names, condition) {
  parse_filter_(names, condition[[1]]$expr, condition[[1]]$env)
}

parse_filter_ <- function(names, expr, env) {
  parsed <- expr
  if (is.call(expr)) {
    op = expr[[1]]
    #print(paste('call:op',op,sep='='))
    parsed <- switch(as.character(op),
                     '&'= parse_logical(names,expr,env),
                     '|'= parse_logical(names,expr,env),
                     '('= parse_logical(names,expr,env),
                     '>'= parse_operator(names,expr,env),
                     '<'= parse_operator(names,expr,env),
                     '=-'= parse_operator(names,expr,env),
                     '<='= parse_operator(names,expr,env),
                     '>='= parse_operator(names,expr,env),
                     stop("Unsupported operator ", op, call. = FALSE))
  } else if (is.name(expr)) {
    #print(paste('name',expr,sep=':'))
    if (!(as.character(expr) %in% names)) {
      parsed <- eval(expr, env)
    } 
  } else if (is.atomic(expr)) {
    ##print(paste('atomic',expr,sep=':'))
  } else {
    stop("Unexpected type ", typeof(expr), call. = FALSE)
  }
  parsed
}

parse_logical <- function(names,expr,env) {
  parsed <- expr
  for(i in 2:length(expr)) {
    #print(paste('call arguments',expr[[i]],typeof(expr[[i]]),class(expr[[i]]),sep=':'))
    parsed[[i]] <- parse_filter_(names,expr[[i]],env)
  }
  
  op <- as.character(expr[[1]])
  
  parsed[[1]] <- switch(op,
                        '&' = as.name('sindex_and'),
                        '|' = as.name('sindex_or'),
                        op)  
  
  parsed
}


parse_operator <- function(names,expr,env) {
  parsed <- expr
  if(length(expr) != 3) {
    stop(expr[[1]], " requires two arguments ", expr, call. = FALSE)
  }
  left <- parse_filter_(names,expr[[2]],env)
  right <- parse_filter_(names,expr[[3]],env)
  
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
  parsed[[1]] <- switch(as.character(parsed[[1]]),
                        '=' = as.name('op_eq'),
                        '>' = as.name('op_gt'),
                        '>=' = as.name('op_ge'),
                        '<' = as.name('op_lt'),
                        '<=' = as.name('op_le'),
                        parsed[[1]])
  parsed
}

 
columns_from_filter<- function(expr, names) {  
  if (is.call(expr)) {
    for(i in 2:length(expr)) {
       names <- columns_from_filter(expr[[i]], names)
    }
  } else if (is.name(expr)) {
     if (!(as.character(expr) %in% names)) {
        names <- c(names, as.character(expr)) 
     }
  } 
  names
}


