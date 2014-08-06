context("Scythica Dataframe")

test_that("Dataframe Basics", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    x <- new(sdf2,path="../extdata/airline.sds")

    expect_that(x$ncol(), equals(29))

    expect_that(x$names()[3], equals("DayOfMonth"))
    expect_that(x$names()[19], equals("Distance"))
    
    expect_that(x$col_index("Distance"), equals(19))
    expect_that(x$col_index("Zulu"), equals(0))

    expect_that(length(x$partitions()), equals(1))
    expect_that(x$partitions()[1], equals("2008-01-03"))

    expect_that(x$partition_rows('2008-01-03'), equals(999))
    expect_that(x$partition_splits('2008-01-03'), equals(2))
    
    ct <- x$col_types()
    expect_that(length(ct), equals(29))
    expect_that(as.character(ct[19]), equals("int32"))
    expect_that(names(ct)[19], equals("Distance"))
    
    cattr <- x$col_attributes()
    expect_that(length(cattr), equals(29))
    expect_that(as.character(cattr[3]), equals("pkey0p2"))
    expect_that(names(ct)[3], equals("DayOfMonth"))   
    
    

    # Test integer vectods
    v <- x$split('2008-01-03',1,19)

    expect_that(length(v), equals(500))
    expect_that(sum(v), equals(323591))
    expect_that(v[100], equals(1987))
    
    v <- x$splitn('2008-01-03',1,'Distance')
    expect_that(length(v), equals(500))
    expect_that(sum(v), equals(323591))
    expect_that(v[100], equals(1987))
    
    v <- x$split('2008-01-03',2,19)
    expect_that(length(v), equals(499))
    expect_that(class(v), equals('integer'))
    
    # Test logical
    v <- x$split('2008-01-03',1,22)
    expect_that(length(v), equals(500))
    expect_that(class(v), equals('logical')) 
    expect_that(v[179], is_true())
    expect_that(v[177], is_false())

    # Test factors
    v <- x$split('2008-01-03',1,11)
    expect_that(class(v), equals('factor'))    
    expect_that(as.character(v[5]), equals('N464WN'))
    expect_that(length(levels(v)), equals(444))   
    
    # Test NA
    v <- x$split('2008-01-03',1,29)
    expect_that(v[5], equals(32))
    expect_true(is.na(v[6]))
    

})

test_that("Dataframe From Split", {
  sdf <- open_sdataset("../extdata/airline.sds")
  # Test data frames
  df <- as_data_frame_split(sdf,'2008-01-03')
  expect_that(nrow(df), equals(500))
# not every column is supported today
  expect_that(ncol(df), equals(22))   
  expect_that(names(df)[13], equals('TaxiIn'))
  expect_that(df[23,13], equals(2))   
  expect_that(df[23,"TaxiIn"], equals(2))   
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
    
    v <- x$split('Iris-virginica',1,4)
    expect_true(is.na(v[47]))
    expect_that(v[25], equals(2.1))
    
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
  expect_true(is.na(v[20]))
  expect_false(is.na(v[19]))
  
})

test_that("Dataset Noaa", {
  
  scy <- Module("rscythica", PACKAGE="RScythica")
  sdf2 <- scy$SDataframe
  noaa <- new(sdf2,path="../extdata/noaa.sds")
  
  expect_that(noaa$ncol(), equals(10))
  
  expect_that(noaa$names()[6], equals("DATE"))
  
  expect_that(length(noaa$partitions()), equals(1))
  expect_that(noaa$partitions()[1], equals("COOP_311564"))
  
  # test date
  v <- noaa$split('COOP_311564',1,6)
  attr(v,"tzone") <- "UTC"
  
  expect_that(length(v), equals(158))
  expect_that(class(v)[2], equals("POSIXct"))
  
  expect_that(as.character(v[12]), 
              equals(as.character(as.POSIXct("1984-01-10 09:00:00",tz="UTC"))))
  expect_true(is.na(v[35]))
  expect_false(is.na(v[34]))            
  
})

test_that("Dataframe Iterator", {
    df <- open_sdataset("../extdata/airline.sds")

    it <- sdf_iterator(df)
    c <- nextElem(it)

    expect_that(c$pkey,equals("2008-01-03"))
    expect_that(c$split,equals(1))

    s <- foreach(i=sdf_iterator(df),.combine=sum) %do% sum(df$split(i$pkey,i$split,19))
    
    expect_that(s,equals(323591))


})


test_that("Dataframe R functions", {
  
  scy <- Module("rscythica", PACKAGE="RScythica")
  sdf2 <- scy$SDataframe
  x <- new(sdf2,path="../extdata/airline.sds")
  
  v <- x["2008-01-03:1","AirTime"]
  
  expect_that(v[484], equals(68))
  
 
  
})