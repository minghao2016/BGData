#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <Rcpp.h>

// [[Rcpp::export]]
unsigned int get_line_count(std::string path, bool has_header, bool is_gzip_compressed) {
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    try {
        boost::iostreams::filtering_istream in;
        if (is_gzip_compressed) {
            in.push(boost::iostreams::gzip_decompressor());
        }
        in.push(file);
        unsigned int n = 0;
        std::string line;
        for (; std::getline(in, line); ++n);
        if (has_header) --n;
        return n;
    } catch(const boost::iostreams::gzip_error& e) {
        Rcpp::stop("Error opening file.");
    }
}
