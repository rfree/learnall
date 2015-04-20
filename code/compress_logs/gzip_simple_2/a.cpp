#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "zlib.h"

class cSimpleCompress {
	public:
		void CompressFile(const std::string & fname_in);
		void CompressFile(const std::string & fname_in, const std::string & fname_out);
};

void cSimpleCompress::CompressFile(const std::string & fname_in) {
	CompressFile(fname_in, fname_in+".gz");
}

void cSimpleCompress::CompressFile(const std::string & fname_in, const std::string & fname_out) {	
	const size_t compress_buffer_size = 1024*128;
	const size_t copy_buffer_size = 1024*128;
	gzFile gz_file = gzopen( fname_out.c_str(), "wb9");
	if ( gz_file == NULL) throw std::runtime_error( std::string("gzip: can not open file '") + fname_out + std::string("'") );
	if ( gzbuffer(gz_file , compress_buffer_size)  != 0) throw std::runtime_error("gzip: can not set gzip compress size");

	std::ifstream input_file;
	input_file.open(fname_in.c_str(), std::ios::binary);

	char buf[copy_buffer_size];
	input_file.read(buf, copy_buffer_size);
	unsigned int len_read = input_file.gcount();
	if (len_read) {
		auto len_written = gzwrite(gz_file, buf, len_read);
		if (len_written != len_read) throw std::runtime_error("gzip: can not write some of the data");
	}
	gzclose(gz_file);
}

int main() {
	cSimpleCompress compress;
	compress.CompressFile("test");
}


