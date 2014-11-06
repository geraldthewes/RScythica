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
