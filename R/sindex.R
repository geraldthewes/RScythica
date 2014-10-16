# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.


#' Create an sindex
#'
#' @param size of index
#' @return Empty index for a split 
#' @examples
#'  index <- sindex(500)
#' @export
sindex <- function(size) {
  si <- raw(size)
  class(si) <- c('sindex','raw')
  si
}

#' Assign Value
#'
#' @param size of index
#' @return Empty index for a split 
#' @examples
#'  index <- sindex(500)
#' @export
#"[.sindex" <- function(b, r, v) {
#  v <- strsplit(i,':')
#  p <- v[[1]][1]
#  split <- as.numeric(v[[1]][2])
#  x$splitn(p,split,j)
#}