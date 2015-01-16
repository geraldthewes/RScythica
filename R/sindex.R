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

# Assign Value
#
# @param size of index
# @return Empty index for a split 
# @examples
#  index <- sindex(500)
# @export
#"[.sindex" <- function(b, r, v) {
#  v <- strsplit(i,':')
#  p <- v[[1]][1]
#  split <- as.numeric(v[[1]][2])
#  x$splitn(p,split,j)
#}

#' And two indexes
#'
#' @param left 
#' @param right
#' @param .index optionally pass in an index that will be used instead of allocating a new one
#' @return And of left and right 
#' @examples
#'  a <- sindex(500)
#'  b <- sindex(500)
#'  c <- sindex_and(a,b)
#' @export
sindex_and <- function(left, right, .index=NULL) {
  assert_that (length(left) == length(right))
  index.left <- new(s.factory$m$BitVector,left)
  if (is.null(.index)) {
    #.index <- sindex(length(left))
    .index <- left
  } else {
    assert_that (length(left) == length(.index))
  }
  index.out <- index.left$op.and(.index, right) 
}



#' Or two indexes
#'
#' @param left 
#' @param right
#' @param .index optionally pass in an index that will be used instead of allocating a new one
#' @return Or of left and right 
#' @examples
#'  a <- sindex(500)
#'  b <- sindex(500)
#'  c <- sindex_or(a,b)
#' @export
sindex_or <- function(left, right, .index=NULL) {
  assert_that (length(left) == length(right))

  index.left <- new(s.factory$m$BitVector,left)
  if (is.null(.index)) {
    #.index <- sindex(length(left))
    .index <- left
  } else {
    assert_that (length(left) == length(.index))
  }
  index.out <- index.left$op.or(.index, right) 
}

#' Andnot two indexes
#'
#' @param left 
#' @param right
#' @param .index optionally pass in an index that will be used instead of allocating a new one
#' @return And left and not right 
#' @examples
#'  a <- sindex(500)
#'  b <- sindex(500)
#'  c <- sindex_andnot(a,b)
#' @export
sindex_andnot <- function(left, right, .index=NULL) {
  assert_that (length(left) == length(right))

  index.left <- new(s.factory$m$BitVector,left)
  if (is.null(.index)) {
    #.index <- sindex(length(left))
    .index <- left
  } else {
    assert_that (length(left) == length(.index))
  }
  index.out <- index.left$op.andnot(.index, right) 
}

#' Not index
#'
#' @param a - a is modified
#' @return Not a
#' @examples
#'  a <- sindex(500)
#'  b <- sindex_not(a)
#' @export
sindex_not <- function(a) {
  index.a <- new(s.factory$m$BitVector,a)
  index.out <- index.a$op.not() 
}

#' population count
#'
#' @param a - a is modified
#' @return Not a
#' @examples
#'  a <- sindex(500)
#'  n <- sindex_popcount(a)
#' @export
sindex_popcount <- function(a) {
  index.a <- new(s.factory$m$BitVector,a)
  index.out <- index.a$popcount() 
}