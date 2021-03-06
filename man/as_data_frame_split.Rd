% Generated by roxygen2 (4.0.2): do not edit by hand
\name{as_data_frame_split}
\alias{as_data_frame_split}
\title{Return a split as a dataframe}
\usage{
as_data_frame_split(sds, partition, split = 1)
}
\arguments{
\item{sds}{S Dataframe}

\item{partition}{key}

\item{split}{index}
}
\value{
Returns the split as a dataframe
}
\description{
Access a split as a dataframe
}
\examples{
\dontrun{
 df <- as_data_frame_split(airline,'2008-01-03',1)
}
}

