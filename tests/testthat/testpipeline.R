context("Scythica Dataframe Pipeline")



test_that("Test Basic Pipeline", {
  sdf <- open_sdataset("../extdata/airline.sds")
  l <- 5
  columns <- c('Distance','TaxiIn')
 
  v <- sview(sdf)
  v <- sv_partitions(v,c("2008-01-03"))

  v <- sv_subset(v,Distance, TaxiIn)
  v <- sv_filter(v,Distance > 2000 & TaxiIn < l)

  ds2 <- sv_collect(v)
  
  expect_that(nrow(ds2), testthat::equals(10))  
  expect_that(sv_rows(v), testthat::equals(10))  
  expect_that(ncol(ds2), testthat::equals(2)) 
  expect_that(names(ds2),testthat::equals(columns))
  
  expect_that(sum(ds2$Distance), testthat::equals(22265))
  expect_that(min(ds2$Distance), testthat::equals(2027))
  
  expect_that(sum(ds2$TaxiIn), testthat::equals(35))
  expect_that(max(ds2$TaxiIn), testthat::equals(4))

})

test_that("Test Basic Pipeline - 2 ", {
  sdf <- open_sdataset("../extdata/airline.sds")
  l <- 5
  columns <- c('Distance','TaxiIn')
  
  v <- sview(sdf)
  v <- sv_partitions(v,c("2008-01-03"))
  
  v <- sv_subset(v,Distance, TaxiIn)
  v <- sv_filter(v,l > TaxiIn  & 2000 < Distance)
  
  ds2 <- sv_collect(v)
  
  expect_that(nrow(ds2), testthat::equals(10))  
  expect_that(sv_rows(v), testthat::equals(10))  
  expect_that(ncol(ds2), testthat::equals(2)) 
  expect_that(names(ds2),testthat::equals(columns))
  
  expect_that(sum(ds2$Distance), testthat::equals(22265))
  expect_that(min(ds2$Distance), testthat::equals(2027))
  
  expect_that(sum(ds2$TaxiIn), testthat::equals(35))
  expect_that(max(ds2$TaxiIn), testthat::equals(4))
  
})

test_that("Test All Columns ", {
  sdf <- open_sdataset("../extdata/airline.sds")
  
  v <- sview(sdf)
  v <- sv_partitions(v,c("2008-01-03"))
  
  v <- sv_filter(v,Distance > 2000)
  
  ds2 <- sv_collect(v)
  
  expect_that(nrow(ds2), testthat::equals(19))  
  expect_that(ncol(ds2), testthat::equals(29)) 
  
})

