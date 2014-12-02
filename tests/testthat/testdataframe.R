context("Scythica Dataframe")

test_that("Dataframe Basics", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    x <- new(sdf2,path="../extdata/airline.sds")

    expect_that(x$ncol(), testthat::equals(29))
    expect_that(x$key_separator, testthat::equals("-"))
    
    expect_that(x$partitions_column_value('2001-08-03', 'Year'), testthat::equals('2001'))
    expect_that(x$partitions_column_value('2001-08-03', 'Month'), testthat::equals('08'))
    expect_that(x$partitions_column_value('2001-08-03', 'DayOfMonth'), testthat::equals('03'))
    expect_that(x$partitions_column_value('2001-08-03', 'Day'), testthat::equals(''))
    
    expect_that(x$names()[3], testthat::equals("DayOfMonth"))
    expect_that(x$names()[19], testthat::equals("Distance"))
    
    expect_that(x$col_index("Distance"), testthat::equals(19))
    expect_that(x$col_index("Zulu"), testthat::equals(0))

    expect_that(length(x$partitions()), testthat::equals(1))
    expect_that(x$partitions()[1], testthat::equals("2008-01-03"))

    expect_that(x$partition_rows('2008-01-03'), testthat::equals(999))
    expect_that(x$partition_splits('2008-01-03'), testthat::equals(2))
    
    ct <- x$col_types()
    expect_that(length(ct), testthat::equals(29))
    expect_that(as.character(ct[19]), testthat::equals("int32"))
    expect_that(names(ct)[19], testthat::equals("Distance"))
    
    cattr <- x$col_attributes()
    expect_that(length(cattr), testthat::equals(29))
    expect_that(as.character(cattr[3]), testthat::equals("pkey0p2"))
    expect_that(names(ct)[3], testthat::equals("DayOfMonth"))   
    
    l <- x$col_levels("Dest")
    expect_that(length(l), testthat::equals(62))
    expect_that(as.character(l[1]), testthat::equals("TPA"))     

    # Test integer vectors
    v <- x$split('2008-01-03',1,19)

    expect_that(length(v), testthat::equals(500))
    expect_that(sum(v), testthat::equals(323591))
    expect_that(v[100], testthat::equals(1987))
    
    v <- x$splitn('2008-01-03',1,'Distance')
    expect_that(length(v), testthat::equals(500))
    expect_that(sum(v), testthat::equals(323591))
    expect_that(v[100], testthat::equals(1987))
    
    v <- x$split('2008-01-03',2,19)
    expect_that(length(v), testthat::equals(499))
    expect_that(class(v), testthat::equals('integer'))
    
    # Test logical
    v <- x$split('2008-01-03',1,22)
    expect_that(length(v), testthat::equals(500))
    expect_that(class(v), testthat::equals('logical')) 
    expect_that(v[179], is_true())
    expect_that(v[177], is_false())

    # Test factors
    v <- x$split('2008-01-03',1,11)
    expect_that(class(v), testthat::equals('factor'))    
    expect_that(as.character(v[5]), testthat::equals('N464WN'))
    expect_that(length(levels(v)), testthat::equals(444))   
    
    # Test NA
    v <- x$split('2008-01-03',1,29)
    expect_that(v[5], testthat::equals(32))
    expect_true(is.na(v[6]))
    
    # Test Partition Columns
    v <- x$splitn('2008-01-03',1,'Year')
    expect_that(length(v), testthat::equals(500))
    expect_that(sum(v), testthat::equals(1004000))
    expect_that(v[100], testthat::equals(2008))

    v <- x$splitn('2008-01-03',1,'Month')
    expect_that(length(v), testthat::equals(500))
    expect_that(sum(v), testthat::equals(500))
    expect_that(v[100], testthat::equals(1))
    
    v <- x$splitn('2008-01-03',1,'DayOfMonth')
    expect_that(length(v), testthat::equals(500))
    expect_that(sum(v), testthat::equals(1500))
    expect_that(v[100], testthat::equals(3))
})

test_that("Dataframe From Split", {
  sdf <- open_sdataset("../extdata/airline.sds")
  # Test data frames
  df <- as_data_frame_split(sdf,'2008-01-03')
  expect_that(nrow(df), testthat::equals(500))
# not every column is supported today
  expect_that(ncol(df), testthat::equals(25))   
  expect_that(names(df)[16], testthat::equals('TaxiIn'))
  expect_that(df[23,16], testthat::equals(2))   
  expect_that(df[23,"TaxiIn"], testthat::equals(2))   
})

test_that("Dataset Iris", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    x <- new(sdf2,path="../extdata/iris.sds")

    expect_that(x$ncol(), testthat::equals(5))

    expect_that(x$names()[3], testthat::equals("Petal.Length"))

    expect_that(length(x$partitions()), testthat::equals(3))
    expect_that(x$partitions()[1], testthat::equals("Iris-setosa"))

    expect_that(x$partition_rows('Iris-setosa'), testthat::equals(50))
    expect_that(x$partition_splits('Iris-setosa'), testthat::equals(1))

    # test double vectors
    v <- x$split('Iris-setosa',1,1)

    expect_that(length(v), testthat::equals(50))
    expect_that(sum(v), testthat::equals(250.3))
    expect_that(v[25], testthat::equals(4.8))
    
    v <- x$split('Iris-virginica',1,4)
    expect_true(is.na(v[47]))
    expect_that(v[25], testthat::equals(2.1))
    
    d <- x$partitions_range('A','Z')
    expect_is(d,"data.frame")
    expect_that(nrow(d), testthat::equals(3L))
    expect_that(sum(d$rows), testthat::equals(150L))
    expect_that(sum(d$splits), testthat::equals(3L))
    expect_that(sum(d$remainder), testthat::equals(150L))
    
    d <- x$partitions_regex('Iris-v.*')
    expect_is(d,"data.frame")
    expect_that(nrow(d), testthat::equals(2L))
    expect_that(sum(d$rows), testthat::equals(100L))  
    expect_that(sum(d$splits), testthat::equals(2L))
    expect_that(sum(d$remainder), testthat::equals(100L))
    
})

test_that("Dataset Boston", {
  
  scy <- Module("rscythica", PACKAGE="RScythica")
  sdf2 <- scy$SDataframe
  x <- new(sdf2,path="../extdata/boston.sds")
  
  expect_that(x$ncol(), testthat::equals(4))
  
  expect_that(x$names()[2], testthat::equals("dt"))
  
  expect_that(length(x$partitions()), testthat::equals(1))
  expect_that(x$partitions()[1], testthat::equals("boston"))
    
  # test date
  v <- x$split('boston',1,2)
  
  expect_that(length(v), testthat::equals(44))
  expect_that(class(v), testthat::equals("Date"))
  expect_that(v[12], testthat::equals(as.Date("1982-01-01")))
  expect_true(is.na(v[20]))
  expect_false(is.na(v[19]))
  
})

test_that("Dataset Noaa", {
  
  scy <- Module("rscythica", PACKAGE="RScythica")
  sdf2 <- scy$SDataframe
  noaa <- new(sdf2,path="../extdata/noaa.sds")
  
  expect_that(noaa$ncol(), testthat::equals(10))
  
  expect_that(noaa$names()[6], testthat::equals("DATE"))
  
  expect_that(length(noaa$partitions()), testthat::equals(1))
  expect_that(noaa$partitions()[1], testthat::equals("COOP_311564"))
  
  # test date
  v <- noaa$split('COOP_311564',1,6)
  attr(v,"tzone") <- "UTC"
  
  expect_that(length(v), testthat::equals(158))
  expect_that(class(v)[2], testthat::equals("POSIXct"))
  
  expect_that(as.character(v[12]), 
              testthat::equals(as.character(as.POSIXct("1984-01-10 09:00:00",tz="UTC"))))
  expect_true(is.na(v[35]))
  expect_false(is.na(v[34]))            
  
})

test_that("Dataframe Iterator", {
    df <- open_sdataset("../extdata/airline.sds")

    it <- sdf_iterator(df)
    c <- nextElem(it)

    expect_that(c$pkey,testthat::equals("2008-01-03"))
    expect_that(c$split,testthat::equals(1))

    s <- foreach(i=sdf_iterator(df),.combine=sum) %do% sum(df$split(i$pkey,i$split,19))
    
    expect_that(s,testthat::equals(323591))


})


test_that("Dataframe R functions", {
  
  scy <- Module("rscythica", PACKAGE="RScythica")
  sdf2 <- scy$SDataframe
  x <- new(sdf2,path="../extdata/airline.sds")
  
  v <- x["2008-01-03:1","AirTime"]
  
  expect_that(v[484], testthat::equals(68))
  
 
  
})