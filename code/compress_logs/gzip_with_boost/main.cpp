#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>


void handle_gzip() {
	using namespace boost::iostreams;

	std::ofstream ofile(filename.c_str(), std::ios_base::out | std::ios_base::binary);

	filtering_ostream out;
	out.push(gzip_compressor());
	out.push(ofile);

	out << "Some gzipped log";
}

int main() {
	handle_gzip();
}