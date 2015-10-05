#' BGData: Memory Mapped Matrices and Data-Structures for Genomic Data for R
#'
#' More info in the wiki: \url{https://github.com/QuantGen/BGData/wiki}
#'
#' @docType package
#' @name BGData
#' @import methods
#' @import parallel
#' @import ff
#' @import Rcpp
#' @importClassesFrom LinkedMatrix LinkedMatrix
#' @useDynLib BGData
loadModule("mod_BEDMatrix", TRUE)
