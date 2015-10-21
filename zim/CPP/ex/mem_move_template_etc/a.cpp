#include "lib.hpp"

#define USE_TEMPLATES 0

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

using namespace std;

volatile long long int a=42;

int crap_rand() {
	volatile auto b=a++;
	int r = b*101101 % 54101;
	if (r<0) r = -r;
	return r;
}



class c_encr { public:
	virtual void use(string &s)=0;
	virtual char use_c(char c)=0;
};


class c_encr_a : public c_encr { public:
	virtual void use(string &s);
	virtual char use_c(char c);
};

void c_encr_a::use(string &s) {
	const size_t len=s.size();
	// TODO not so safe coding style about the limit 
	// (actually safe, as % will at most decrease the inded)
	for (size_t i=0; i<len; ++i) s[i] = use_c( s[i] + s[i%1000] + s[i%2000] 
		+s[i%3000]);
}

char c_encr_a::use_c(char c) {
	return c+'x';
}

class c_encr_b : public c_encr { public:
	virtual void use(string &s);
	virtual char use_c(char c);
};

void c_encr_b::use(string &s) {
	const size_t len=s.size();
	// TODO not so safe coding style about the limit 
	// (actually safe, as % will at most decrease the inded)
	for (size_t i=0; i<len; ++i) s[i] = use_c( s[i] + s[i%1000] + s[i%2000] 
		+s[i%3000]);
}


char c_encr_b::use_c(char c) {
	return c+'x';
}

#if !USE_TEMPLATES

class c_node {
	public:
		c_encr *m_encr;

		void hello() const;
		void test(std::string &s);
};

void c_node::hello() const {
	cout << (this) << " I use pointer " << endl;
}

void c_node::test(std::string &s) {
	m_encr->use(s);
}

void do_tests(string &dataset, bool say_hello) {
	c_node node;
	node.m_encr = new c_encr_a; 

	if (say_hello) node.hello();
	node.test( dataset );

	delete node.m_encr;
}

#else


template <class T_ENCR>
class c_node {
	public:
		T_ENCR m_encr;

		void hello() const;
		void test(std::string &s);
};


template <class T_ENCR>
void c_node<T_ENCR>::hello() const {
	cout << (void*)(this) << " I use template " << endl;
}

template <class T_ENCR>
void c_node<T_ENCR>::test(std::string &s) {
	m_encr.use(s);
}

void do_tests(string &dataset, bool say_hello) {
	c_node<c_encr_a> node;
//	node.m_encr = new c_encr_a; 

	if (say_hello) node.hello();
	node.test( dataset );

//	delete node.m_encr;
}

#endif

int main() {
	a += time(NULL);
	a = 42;



	vector<string> testdata;
	for (int test_nr=0; test_nr<10; ++test_nr) {
		testdata.push_back(
			string(10*1000, (char)( crap_rand() % 256) )
		);
	}
	cout << "Test data ready" << endl;

	
	double totall_time=0;
	int test_run=0; 
	for (test_run=0; test_run<2; ++test_run) {

	boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::local_time();
	for (int test_nr=0; test_nr<1000; ++test_nr) {
		auto & dataset = testdata.at( test_nr % testdata.size() );
		bool say_hello = test_nr==0 && test_run==0 ;
		do_tests(dataset, say_hello);
	}

	boost::posix_time::ptime t2 = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration diff = t2 - t1;
	auto millisec = diff.total_milliseconds();
	std::cout << "Run: " << millisec << " ms" << std::endl;
	totall_time += millisec;

	}
	cout << "Test run: " << test_run << endl;
	double avg_time = totall_time / test_run;

	cout << "AVG TIME: " << avg_time << endl;

	
}
