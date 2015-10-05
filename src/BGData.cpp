#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/regex.hpp>
#include <Rcpp.h>

void splitString(std::vector<std::string>& container, const std::string& str, const std::string& delim) {
    boost::regex splitter(delim);
    boost::sregex_token_iterator iter(str.begin(), str.end(), splitter, -1);
    boost::sregex_token_iterator iterEnd;
    for (; iter != iterEnd; ++iter) {
        container.push_back(iter->str());
    }
}

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

// [[Rcpp::export]]
std::vector<std::string> get_file_header(std::string path, bool is_gzip_compressed) {
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    try {
        boost::iostreams::filtering_istream in;
        if (is_gzip_compressed) {
            in.push(boost::iostreams::gzip_decompressor());
        }
        in.push(file);
        std::string line;
        std::getline(in, line);
        std::vector<std::string> splits;
        splitString(splits, line, "\\s+");
        return splits;
    } catch(const boost::iostreams::gzip_error& e) {
        Rcpp::stop("Error opening file.");
    }
}
