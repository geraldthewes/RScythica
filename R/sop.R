# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.


#' Greater Than
#'
#' @param v vector
#' @param value vlaue to compare
#' @return index 
#' @examples
#'  index <- op_gt(ds$splitn("2001-01-01",1,'Distance'),1000)
#' @export
op_gt <- function(v,value) {
  UseMethod("op_gt")
}


#' @export
op_gt.integer <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SIntVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.gt(v, sindex, value)
}

#' @export
op_gt.numeric <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SNumericVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.gt(v, sindex, value)
}

#' Equal
#'
#' @param v vector
#' @param value vlaue to compare
#' @return index 
#' @examples
#'  index <- op_eq(ds$splitn("2001-01-01",1,'Distance'),1000)
#' @export
op_eq <- function(v,value) {
  UseMethod("op_eq")
}


#' @export
op_eq.integer <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SIntVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.eq(v, sindex, value)
}

#' @export
op_eq.numeric <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SNumericVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.eq(v, sindex, value)
}

#' Less Than
#'
#' @param v vector
#' @param value vlaue to compare
#' @return index 
#' @examples
#'  index <- op_lt(ds$splitn("2001-01-01",1,'Distance'),1000)
#' @export
op_lt <- function(v,value) {
  UseMethod("op_lt")
}


#' @export
op_lt.integer <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SIntVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.lt(v, sindex, value)
}

#' @export
op_lt.numeric <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SNumericVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.lt(v, sindex, value)
}

#' Greater Than or Equal
#'
#' @param v vector
#' @param value vlaue to compare
#' @return index 
#' @examples
#'  index <- op_ge(ds$splitn("2001-01-01",1,'Distance'),1000)
#' @export
op_ge <- function(v,value) {
  UseMethod("op_ge")
}


#' @export
op_ge.integer <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SIntVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.ge(v, sindex, value)
}

#' @export
op_ge.numeric <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SNumericVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.ge(v, sindex, value)
}

#' Less Than or Equal
#'
#' @param v vector
#' @param value vlaue to compare
#' @return index 
#' @examples
#'  index <- op_le(ds$splitn("2001-01-01",1,'Distance'),1000)
#' @export
op_le <- function(v,value) {
  UseMethod("op_le")
}


#' @export
op_le.integer <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SIntVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.le(v, sindex, value)
}

#' @export
op_le.numeric <- function(v,value) {
  m   <- Module("rscythica", PACKAGE="RScythica")
  vfactory <- m$SNumericVector
  bm.v <- new(vfactory)
  sindex <- sindex(length(v))
  b <- bm.v$op.le(v, sindex, value)
}


