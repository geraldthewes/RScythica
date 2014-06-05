context("Scythica Dataframe")

test_that("Dataframe Basics", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    x <- new(sdf2,path="/home3/gth/go/src/github.com/geraldthewes/scythica/sdsmeta/airline")

    expect_that(x$ncol(), equals(29))

    expect_that(x$names()[3], equals("DayOfMonth"))

    expect_that(x$partitions()[1], equals("2008-01-03"))

    expect_that(x$partition_rows('2008-01-03'), equals(999))

    v <- x$split('2008-01-03',1,1)

    expect_that(length(v), equals(500))
    
    v <- x$split('2008-01-03',2,1)

    expect_that(length(v), equals(499))
})


test_that("Dataframe Iterator", {
    
    scy <- Module("rscythica", PACKAGE="RScythica")
    sdf2 <- scy$SDataframe
    df <- new(sdf2,path="/home3/gth/go/src/github.com/geraldthewes/scythica/sdsmeta/airline")

    it <- sdf_iterator(df)
    c <- nextElem(it)

    expect_that(c$pkey,equals("2008-01-03"))
    expect_that(c$split,equals(1))

    s <- foreach(i=sdf_iterator(df),.combine=sum) %do% sum(df$split(i$pkey,i$split,19))
    
    expect_that(s,equals(323591))


})
