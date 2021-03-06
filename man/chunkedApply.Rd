% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/utils.R
\name{chunkedApply}
\alias{chunkedApply}
\title{Applies a Function on Each Row or Column of a Memory-Mapped Matrix-Like
Object.}
\usage{
chunkedApply(X, MARGIN, FUN, i = seq_len(nrow(X)), j = seq_len(ncol(X)),
  bufferSize = 5000L, nTasks = nCores, nCores = getOption("mc.cores", 2L),
  verbose = FALSE, ...)
}
\arguments{
\item{X}{A memory-mapped matrix-like object, typically \code{@geno} of a
\linkS4class{BGData} object.}

\item{MARGIN}{The subscripts which the function will be applied over. 1
indicates rows, 2 indicates columns.}

\item{FUN}{The function to be applied.}

\item{i}{Indicates which rows of \code{X} should be used. Can be integer,
boolean, or character. By default, all rows are used.}

\item{j}{Indicates which columns of \code{X} should be used. Can be integer,
boolean, or character. By default, all columns are used.}

\item{bufferSize}{The number of rows or columns of \code{X} that are brought into
RAM for processing. If \code{NULL}, all elements in \code{i} or \code{j} are used. Defaults
to 5000.}

\item{nTasks}{The number of tasks the problem should be broken into to be
distributed among \code{nCores} cores. Defaults to \code{nCores}.}

\item{nCores}{The number of cores (passed to \code{\link[parallel:mclapply]{parallel::mclapply()}}).
Defaults to the number of cores as detected by \code{\link[parallel:detectCores]{parallel::detectCores()}}.}

\item{verbose}{Whether progress updates will be posted. Defaults to \code{FALSE}.}

\item{...}{Additional arguments to be passed to the \code{\link[base:apply]{base::apply()}} like
function.}
}
\description{
Similar to \code{\link[base:apply]{base::apply()}}, but designed for memory-mapped matrix-like
objects. The function brings chunks of an object into physical memory by
taking subsets, and applies a function on either the rows or the columns of
the chunks using an optimized version of \code{\link[base:apply]{base::apply()}}. If \code{nTasks} is
greater than 1, the function will be applied in parallel using
\code{\link[parallel:mclapply]{parallel::mclapply()}}. In that case the subsets of the object are taken on
the slaves.
}
\section{Memory-mapping}{

Functions with the \code{bufferSize} parameter work best with memory-mapped
matrices such as \link[BEDMatrix:BEDMatrix-class]{BEDMatrix::BEDMatrix} objects. To avoid loading the
whole, potentially very large matrix into memory, these functions will load
chunks of the memory-mapped matrix into memory and perform the operations on
one chunk at a time. The size of the chunks is determined by the
\code{bufferSize} parameter. Care must be taken to not set \code{bufferSize} too high
to avoid memory shortage, particularly when combined with parallel
computing.
}

\section{Multi-level parallelism}{

Functions with the \code{nCores}, \code{nTasks}, \code{i}, and \code{j} parameters provide
capabilities for both parallel and distributed computing.

For parallel computing, \code{nCores} determines the number of cores the code is
run on, and \code{nTasks} determines into how many tasks the problem is divided
into. \code{nTasks} should be at least as high as \code{nCores} to keep all cores
busy. Memory usage can be an issue for higher values of \code{nCores} and
\code{nTasks} as R is not particularly memory-efficient. As a rule of thumb, at
least around \code{object_size(X) + (nCores * (object_size(X) / nTasks)) + object_size(result)} MB of total memory will be needed for operations on
memory-mapped matrices, not including potential copies of your data that
might be created (for example \code{\link[stats:lsfit]{stats::lsfit()}} runs \code{cbind(1, X)}). \code{i} and
\code{j} can be used to include or exclude certain rows or columns. Internally,
the \code{\link[parallel:mclapply]{parallel::mclapply()}} function is used and therefore parallel computing
will not work on Windows machines.

For distributed computing, \code{i} and \code{j} determine the subset of the input
matrix that the code runs on. In an HPC environment, this can be used not
just to include or exclude certain rows or columns, but also to partition
the task among many nodes rather than cores. Scheduler-specific code and
code to aggregate the results need to be written by the user. It is
recommended to set \code{nCores} and \code{nTasks} to \code{1} as nodes are often cheaper
than cores.
}

\examples{
# Restrict number of cores to 1 on Windows
if (.Platform$OS.type == "windows") {
    options(mc.cores = 1)
}

# Load example data
bg <- BGData:::loadExample()

# Compute standard deviation of columns
chunkedApply(X = bg@geno, MARGIN = 2, FUN = sd)
}
