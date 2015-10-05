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

template <typename T>
class RawParser {
    public:
        RawParser(std::string, unsigned int, unsigned int, std::string, bool);
        Rcpp::List next_line();
    private:
        std::ifstream file;
        unsigned int p;
        unsigned int col_skip;
        std::string na_string;
        boost::iostreams::filtering_istream is;
        void assign_genotypes(T&, const std::vector<std::string>&);
};

template <typename T>
RawParser<T>::RawParser(std::string path, unsigned int p_, unsigned int col_skip_, std::string na_string_, bool is_gzip_compressed) : file(path.c_str(), std::ios::in | std::ios::binary), p(p_), col_skip(col_skip_), na_string(na_string_) {
    try {
        if (is_gzip_compressed) {
            is.push(boost::iostreams::gzip_decompressor());
        }
        is.push(file);
    } catch(const boost::iostreams::gzip_error& e) {
        Rcpp::stop("Error opening file.");
    }
}

template <typename T>
Rcpp::List RawParser<T>::next_line() {
    std::string line;
    if (!std::getline(is, line)) {
        Rcpp::stop("There are no lines left.");
    }
    std::vector<std::string> items;
    splitString(items, line, "\\s+");
    // Assign phenotypes
    Rcpp::CharacterVector pheno(items.begin(), items.begin() + col_skip);
    // Assign genotypes
    T geno(p);
    assign_genotypes(geno, std::vector<std::string>(items.begin() + col_skip, items.end()));
    return Rcpp::List::create(
        Rcpp::_["pheno"] = pheno,
        Rcpp::_["geno"] = geno
    );
}

template <>
void RawParser<Rcpp::IntegerVector>::assign_genotypes(Rcpp::IntegerVector& out, const std::vector<std::string>& in) {
    for (std::vector<std::string>::size_type i = 0; i < in.size(); ++i) {
        std::string el(in[i]);
        if (el == na_string) {
            out[i] = NA_INTEGER;
        } else {
            out[i] = atoi(el.c_str());
        }
    }
}

template <>
void RawParser<Rcpp::DoubleVector>::assign_genotypes(Rcpp::DoubleVector& out, const std::vector<std::string>& in) {
    for (std::vector<std::string>::size_type i = 0; i < in.size(); ++i) {
        std::string el(in[i]);
        if (el == na_string) {
            out[i] = NA_REAL;
        } else {
            out[i] = atof(el.c_str());
        }
    }
}

template <>
void RawParser<Rcpp::CharacterVector>::assign_genotypes(Rcpp::CharacterVector& out, const std::vector<std::string>& in) {
    for (std::vector<std::string>::size_type i = 0; i < in.size(); ++i) {
        std::string el(in[i]);
        if (el == na_string) {
            out[i] = NA_STRING;
        } else {
            out[i] = el;
        }
    }
}

RCPP_MODULE(mod_BEDMatrix) {

    using namespace Rcpp ;

    class_<RawParser<Rcpp::IntegerVector> >("RawParserInt")
    .constructor<std::string, unsigned int, unsigned int, std::string, bool>()
    .method("nextLine", &RawParser<Rcpp::IntegerVector>::next_line)
    ;

    class_<RawParser<Rcpp::NumericVector> >("RawParserDouble")
    .constructor<std::string, unsigned int, unsigned int, std::string, bool>()
    .method("nextLine", &RawParser<Rcpp::NumericVector>::next_line)
    ;

    class_<RawParser<Rcpp::CharacterVector> >("RawParserChar")
    .constructor<std::string, unsigned int, unsigned int, std::string, bool>()
    .method("nextLine", &RawParser<Rcpp::CharacterVector>::next_line)
    ;

}
