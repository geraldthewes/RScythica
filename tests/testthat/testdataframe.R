context("Scythica Dataframe")

test_that("Dataframe Basics", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    x <- new(sdf2,path="../extdata/airline.sds")

    expect_that(x$ncol(), equals(29))

    expect_that(x$names()[3], equals("DayOfMonth"))
    expect_that(x$names()[19], equals("Distance"))

    expect_that(length(x$partitions()), equals(1))
    expect_that(x$partitions()[1], equals("2008-01-03"))

    expect_that(x$partition_rows('2008-01-03'), equals(999))
    expect_that(x$partition_splits('2008-01-03'), equals(2))

    # Test integer vectods
    v <- x$split('2008-01-03',1,19)

    expect_that(length(v), equals(500))
    expect_that(sum(v), equals(323591))
    expect_that(v[100], equals(1987))
    
    v <- x$split('2008-01-03',2,19)
    expect_that(length(v), equals(499))
    expect_that(class(v), equals('integer'))    

    # Test factors
    v <- x$split('2008-01-03',1,11)
    expect_that(class(v), equals('factor'))    
    expect_that(as.character(v[5]), equals('N464WN'))
    expect_that(length(levels(v)), equals(444))    

})


test_that("Dataset Iris", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    x <- new(sdf2,path="../extdata/iris.sds")

    expect_that(x$ncol(), equals(5))

    expect_that(x$names()[3], equals("Petal.Length"))

    expect_that(length(x$partitions()), equals(3))
    expect_that(x$partitions()[1], equals("Iris-setosa"))

    expect_that(x$partition_rows('Iris-setosa'), equals(50))
    expect_that(x$partition_splits('Iris-setosa'), equals(1))

    # test double vectors
    v <- x$split('Iris-setosa',1,1)

    expect_that(length(v), equals(50))
    expect_that(sum(v), equals(250.3))
    expect_that(v[25], equals(4.8))
    
})

test_that("Dataset Boston", {
  
  scy <- Module("rscythica", PACKAGE="RScythica")
  sdf2 <- scy$SDataframe
  x <- new(sdf2,path="../extdata/boston.sds")
  
  expect_that(x$ncol(), equals(4))
  
  expect_that(x$names()[2], equals("dt"))
  
  expect_that(length(x$partitions()), equals(1))
  expect_that(x$partitions()[1], equals("boston"))
    
  # test date
  v <- x$split('boston',1,2)
  
  expect_that(length(v), equals(44))
  expect_that(class(v), equals("Date"))
  expect_that(v[12], equals(as.Date("1982-01-01")))
  
})


test_that("Dataframe Iterator", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    df <- new(sdf2,path="../extdata/airline.sds")

    it <- sdf_iterator(df)
    c <- nextElem(it)

    expect_that(c$pkey,equals("2008-01-03"))
    expect_that(c$split,equals(1))

    s <- foreach(i=sdf_iterator(df),.combine=sum) %do% sum(df$split(i$pkey,i$split,19))
    
    expect_that(s,equals(323591))


})
