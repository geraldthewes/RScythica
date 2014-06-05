sdf_iterator <- function(sdf) {
    pidx <- 1
    split <- 1
    parts <- sdf$partitions()
    
    nextEl <- function() {
        split1 <- split
        pkey <- parts[pidx]

        # Now move pointer to next
        split <<- split + 1
        if (split > sdf$partition_splits(pkey)) {
            pidx <<- pidx + 1
            split <<- 1
            if(pidx > length(parts)) {
                stop('StopIteration')
            }
        }
        
        list(pkey=pkey,split=split1)
    }

    obj <- list(nextElem=nextEl)
    class(obj) <- c('sdf_iterator','abstractiter','iter')
    obj
}
