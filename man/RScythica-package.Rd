\name{RScythica-package}
\alias{RScythica-package}
\alias{RScythica}
\docType{package}
\title{
Implements large scale dataframes
}
\description{
  RScythica Dataframes, called S Dataframes, are large scale,read-only,
  eventually distributed, data frames which are both partioned and
  broken into large splits. S Dataframes are created using external utilities.
}
\details{
\tabular{ll}{
Package: \tab RScythica\cr
Type: \tab Package\cr
Version: \tab 1.0\cr
Date: \tab 2014-06-15\cr
License: \tab LGPL 2.1\cr
}
TK Overview
}
\author{
Gerald Hewes

Maintainer: <gerald@geraldthewes.org>
}
\references{
Distributed R
ff
bigmemory
}
\keyword{ package bigdata }
\seealso{
}
\examples{
library(RScythica)
sdf <- Module("rscythica", PACKAGE="RScythica")
sdf2 <- sdf$SDataframe

#x <- new(sdf2,path="rscythica/tests/extdata/airline.sds")
#x$ncol()
#x$names()
#x$partitions()

#x$partition_rows('2008-01-03')

#v <- x$split('2008-01-03',1,19)

#it <- sdf_iterator(df)
#c <- nextElem(it)

#s <- foreach(i=sdf_iterator(df),.combine=sum) %do% df$split(i$pkey,i$split,19)

}
