context("Scythica Dataframe Filter")



test_that("Test EQ", {
  sdf <- open_sdataset("../extdata/airline.sds")
  v <- sdf$split('2008-01-03',1,19)
  
  expect_that(length(v), equals(500))  
  ob <- sindex(length(v))
  
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

test_that("Test GT", {
  sdf <- open_sdataset("../extdata/airline.sds")
  v <- sdf$split('2008-01-03',1,19)
  
  expect_that(length(v), equals(500))  
  ob <- sindex(length(v))
  
  m   <- Module("rscythica", PACKAGE="RScythica")
  bm <- m$SIntVector
  bm.v <- new(bm)
  
  y <- bm.v$op.gt(v,ob,1000)
  expect_that(length(y), equals(length(v)))
  
  out <- bm.v$collapse(v,y)
  ref <- v[v>1000]
  
  expect_that(length(out), equals(107))
  expect_that(length(ref), equals(107))
  
  
})


test_that("Test SIndex OR", {
  a <- sindex(100)
  a[23] = as.raw(0xff)
  b <- sindex(100)
  b[13] = as.raw(0xff)
  c <- sindex(100)
  
  
  m   <- Module("rscythica", PACKAGE="RScythica")
  bm <- m$BitVector
  bv <- new(bm,a)
  ov <- bv$op.or(c, b)

  bv.o <- new(bm,ov)
  expect_that(bv.o$popcount(), equals(2))
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
  
  
  m   <- Module("rscythica", PACKAGE="RScythica")
  bm <- m$BitVector
  bv <- new(bm,a)
  ov <- bv$op.and(c, b)
  
  bv.o <- new(bm,ov)
  expect_that(bv.o$popcount(), equals(1))
  expect_that(ov[23] > 0, is_true())
  expect_that(ov[13] > 0, is_false())
})

  
