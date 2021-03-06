% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/BGData.R
\name{load.BGData}
\alias{load.BGData}
\title{Loads BGData (and Other) Objects from .RData Files.}
\usage{
load.BGData(file, envir = parent.frame())
}
\arguments{
\item{file}{The name of the .RData file to be loaded.}

\item{envir}{The environment where to load the data.}
}
\description{
This function is similar to \code{\link[base:load]{base::load()}}, but also initializes the
different types of objects that the \code{@geno} slot of a \linkS4class{BGData} object
can take. Currently supported are \code{ff_matrix},
\link[bigmemory:big.matrix-class]{bigmemory::big.matrix}, and \link[BEDMatrix:BEDMatrix-class]{BEDMatrix::BEDMatrix} objects. If
the object is of type \link[LinkedMatrix:LinkedMatrix-class]{LinkedMatrix::LinkedMatrix}, all nodes will be
initialized with their appropriate method.
}
