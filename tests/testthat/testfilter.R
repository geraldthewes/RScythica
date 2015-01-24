context("Scythica Dataframe Filter")



test_that("Test EQ", {
  sdf <- open_sdataset("../extdata/airline.sds")
  v <- sdf$split('2008-01-03',1,19)
  
  expect_that(length(v), testthat::equals(500))  
  ob <- sindex(length(v))
  
  bm.v <- smake_vector('integer')
  #bm <- s.factory$m$SIntVector
  #bm.v <- new(bm)
  
  y <- bm.v$op.eq(v,ob,515)
  expect_that(length(y), testthat::equals(500))
  
  out <- bm.v$collapse(v,y)
  ref <- v[v==515]
  
  expect_that(length(out), testthat::equals(3))
  expect_that(length(ref), testthat::equals(3))
  
  
})

test_that("Test GT", {
  sdf <- open_sdataset("../extdata/airline.sds")
  v <- sdf$split('2008-01-03',1,19)
  
  expect_that(length(v), testthat::equals(500))  
  ob <- sindex(length(v))
  
  #bm <- s.factory$m$SIntVector
  #bm.v <- new(bm)
  bm.v <- smake_vector('integer')
  
  y <- bm.v$op.gt(v,ob,1000)
  expect_that(length(y), testthat::equals(length(v)))
  
  out <- bm.v$collapse(v,y)
  ref <- v[v>1000]
  
  expect_that(length(out), testthat::equals(107))
  expect_that(length(ref), testthat::equals(107))
  
  
})


test_that("Test SIndex OR", {
  a <- sindex(100)
  a[23] = as.raw(0xff)
  b <- sindex(100)
  b[13] = as.raw(0xff)
  c <- sindex(100)
  
  #bm <- s.factory$m$BitVector
  #bv <- new(bm,a)
  bv <- sindex_bitvector(a)
  ov <- bv$op.or(c, b)

  bv.o <- sindex_bitvector(ov)
  expect_that(bv.o$popcount(), testthat::equals(2))
  expect_that(ov[23] > 0, is_true())
  expect_that(ov[13] > 0, is_true())
  
})


test_that("Test SIndex AND", {
  a <- sindex(100)
  a[23] = as.raw(0xff)
  b <- sindex(100)
  b[13] = as.raw(0xff)
  b[23] = as.raw(0xff)
  c <- sindex(100)
  
  bv <- sindex_bitvector(a)
  ov <- bv$op.and(c, b)
  
  bv.o <- sindex_bitvector(ov)
  expect_that(bv.o$popcount(), testthat::equals(1))
  expect_that(ov[23] > 0, is_true())
  expect_that(ov[13] > 0, is_false())
})

test_that("Test SIndex ANDNOT", {
  a <- sindex(100)
  a[23] = as.raw(0xff)
  b <- sindex(100)
  b[13] = as.raw(0xff)
  b[23] = as.raw(0xff)
  c <- sindex(100)
  

  bv <- sindex_bitvector(a)
  ov <- bv$op.andnot(c, b)
  
  bv.o <- sindex_bitvector(ov)
  expect_that(bv.o$popcount(), testthat::equals(1))
  expect_that(ov[13] > 0, is_true())
  expect_that(ov[23] > 0, is_false())
})

test_that("Test SIndex NOT", {
  a <- sindex(100)
  a[23] = as.raw(0xff)
  

  bv <- sindex_bitvector(a)
  
  expect_that(bv$popcount(), testthat::equals(1))
  
  # bitwise not
  ov <- bv$op.not()
  
  expect_that(bv$popcount(), testthat::equals(99))
  expect_that(ov[23] == 0, is_true())
  expect_that(ov[13] > 0, is_true())
  
})  

test_that("Test Factors", {
  sdf <- open_sdataset("../extdata/airline.sds")
  v <- sdf$splitn('2008-01-03',1,'Dest')
  
  expect_that(length(v), testthat::equals(500))  

  y <- op_eq(v,'SJC')  
  
  expect_that(length(y), testthat::equals(500))
  expect_that(sindex_popcount(y), testthat::equals(23))
  
  out <- collapse(v,y)
  
  expect_that(length(out), testthat::equals(23))
  expect_that(class(out), testthat::equals('factor'))
})

test_that("Parse Filter", {
  sdf <- open_sdataset("../extdata/airline.sds")
  
  sfilter <- function(.v, ...) {
    .dos = lazyeval::lazy_dots(...)
  } 
  
  
  v <- sview(sdf)
  filter <- sfilter(v,Distance > 1000)
  p <- parse_filter(sdf$names(),filter)
  expect_that(class(p), testthat::equals('call'))
  expect_that(as.character(p)[1], testthat::equals('op_gt'))
  expect_that(as.character(p)[2], testthat::equals('Distance'))
  expect_that(as.character(p)[3], testthat::equals('1000')) 
  

  filter <- sfilter(v,Distance > 1000 &  20 < TaxiIn)
  p <- parse_filter(sdf$names(),filter)
  expect_that(class(p), testthat::equals('call'))
  expect_that(as.character(p)[1], testthat::equals('sindex_and'))
  expect_that(as.character(p)[2], testthat::equals('op_gt(Distance, 1000)'))
  expect_that(as.character(p)[3], testthat::equals('op_gt(TaxiIn, 20)')) 
  
  l <- 20
  filter <- sfilter(v,(Distance > 1000) &  (l < TaxiIn))
  p <- parse_filter(sdf$names(),filter)
  expect_that(class(p), testthat::equals('call'))
  expect_that(as.character(p)[1], testthat::equals('sindex_and'))
  expect_that(as.character(p[[2]][[1]]), testthat::equals('('))
  expect_that(as.character(p[[2]][[2]]), testthat::equals(c('op_gt','Distance', '1000')))
  expect_that(as.character(p[[3]][[1]]), testthat::equals('('))
  expect_that(as.character(p[[3]][[2]]), testthat::equals(c('op_gt','TaxiIn', '20')))
  
})