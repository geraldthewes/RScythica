context("Scythica Dataframe Filter")



test_that("Test EQ", {
  sdf <- open_sdataset("../extdata/airline.sds")
  v <- sdf$split('2008-01-03',1,19)
  
  expect_that(length(v), equals(500))  
  ob <- raw(500)
  
  m   <- Module("rscythica", PACKAGE="RScythica")
  bm <- m$SIntVector
  bm.v <- new(bm)
  
  y <- bm.v$op.eq(v,ob,515)
  expect_that(length(y), equals(500))
  
  out <- bm.v$collapse(v,y)
  ref <- v[v==515]
  
  expect_that(length(out), equals(3))
  expect_that(length(ref), equals(3))
  
  
})

