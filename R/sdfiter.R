# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

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
