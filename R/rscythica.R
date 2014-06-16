#' The rscythica package.
#'
#' @docType package
#' @name rscythica
#' @useDynLib RScythica
#' @import Rcpp
## @import assertthat
## @importFrom utils head tail
## @importFrom Rcpp cppFunction Rcpp.plugin.maker

loadModule("rscythica", TRUE)

#.onLoad <- function(lib,pkg) {
#	loadRcppModules()
#}
NULL
