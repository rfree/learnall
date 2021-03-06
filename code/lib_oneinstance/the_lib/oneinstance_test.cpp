#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <thread>

#include "liboneinstance.hpp"

// local (this TU) debug macro
#define _info(X) do { std::cerr<<getpid()<<"/"<<(std::this_thread::get_id())<<" "<<X<<std::endl; } while(0)

const std::string program_version="0.1"; // version of the test program
const bool program_version_is_wip = true; // work in progress, or release version

using namespace std;

void ShowUsage() {
	cout << "Usage: " << endl;
	cout << "  programname -h" << endl; // [option_h]
	cout << "    shows the help" << endl;
	cout << "  programname -v" << endl;
	cout << "    shows the program version information" << endl;
	cout << "  programname [appname] [-u | -s | -d ]" << endl;
	cout << "    starts the program in one instance" << endl;
	cout << "    appname - is the name of application (we limit to one instance between same appname programs)" << endl;
	cout << "    -u - allows one instance per system user (this is default)" << endl; // [default]
	cout << "    -s - allows one instance per the system" << endl;
	cout << "    -d - allows one instance per working directory" << endl;
//	cout << "    -L - test: just lock once some test object" << endl;
//	cout << "    -U - test: just unlock once the same test object" << endl;
	cout << "    -X - test: run current test case (for user for developer)" << endl;
	cout << "" << endl;
	cout << "" << endl;
	cout << "" << endl;
}

const std::string GetTestProgramVersion() {
	std::ostringstream oss;
	oss << program_version << ( program_version_is_wip ? "-wip" : "" ) ;
	return oss.str();
}

void ShowTestProgramVersion() {
	cout << "This test program is in version: " << GetTestProgramVersion() << endl;
}



#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
void test2() {
	using namespace boost::interprocess;
	using namespace nOneInstance;

	{
		msg_mutex test1("test");
		msg_mutex test2("test",42);
		msg_mutex test3("test",0);
		msg_mutex test4("test",1000);
	}

	cMyNamedMutex mutex(open_or_create, "fstream_named_mutex");	
	mutex.unlock();
	cout << mutex.try_lock() << endl;
	cout << mutex.try_lock() << endl;
	cout << mutex.try_lock() << endl;
}

void RunDeveloperTest() {
	std::ostringstream oss; oss<<"foobar_t_" << time(NULL);  string name(oss.str());
	msg_mutex m(name.c_str() , 64);

	_info("Lock name="<<name);
	{
		string str("Hello there"); vector<char> vec(str.cbegin(), str.cend()); 
		m.lock_msg(vec);
	}

	{
		string str("Hello there"); vector<char> vec(str.cbegin(), str.cend());
		bool relock = m.try_lock_msg(vec);
		_info("relock="<<relock);
	}

	{
		_info("Unlock");
		const vector<char> msg_v = m.unlock_msg();
		string msg(msg_v.cbegin(), msg_v.cend());
		_info("msg="<<msg);
	}

	{
		string str("Hello there"); vector<char> vec(str.cbegin(), str.cend());
		bool relock = m.try_lock_msg(vec);
		_info("relock="<<relock);
	}


/*
	using namespace boost::interprocess;

	int buf=42;
	boost::interprocess::message_queue Q(create_only, name.c_str() , 1 , sizeof(buf) );
	Q.send( &buf , sizeof(int) , 0);
	size_t recvd_size=0;
	unsigned int prio=0;
	Q.try_receive( &buf , sizeof(int) , recvd_size, prio );
	_info("buf="<<buf<<" recvd_size="<<recvd_size);
*/

	/*
	mMsgQueue.send(ptr_void, sizeof(msgtxt_default), 0);
}

void msg_mutex::lock_msg(const t_msg& msg) {
	mMsgQueue.send( msg.data() , sizeof(msg.at(0)) * msg.size(), 0);
}

bool msg_mutex::try_lock() {
	return mMsgQueue.try_send(&mBuffer, sizeof(int), 0);
}

void msg_mutex::unlock() {
	char dumpbuff;
	boost::interprocess::message_queue::size_type recvd_size;
	unsigned int priority;
	size_t size = sizeof(dumpbuff);
	_info("UNLOCK this="<<this<<" size="<<size);
	if (!mMsgQueue.try_receive(&dumpbuff, size, recvd_size, priority))	{
		throw warning_already_unlocked();
	}
	*/

	_info("all done in this test");
}

int main(int argc, char **argv) {
	// test2(); return 0;

	using namespace std;
	vector<string> args; args.reserve(argc); for (int i=0; i<argc; ++i) args.push_back(argv[i]);

	if (argc>=2) if (args.at(1)=="-h") { ShowUsage(); return 0; }
	if (argc>=2) if (args.at(1)=="-v") { ShowTestProgramVersion(); return 0; }
	if (argc>=2) if (args.at(1)=="-X") { 
		RunDeveloperTest(); 
		_info("Done the test - will exit"); 
		return 0; 
	}

	string application_name="test1";
	if (argc>=2) application_name=args.at(1);

	auto range = nOneInstance::e_range_user; // [default]
	if (argc>=3) {
		auto range_name = args.at(2);
		if (range_name == "-s") range = nOneInstance::e_range_system;
		if (range_name == "-u") range = nOneInstance::e_range_user;
		if (range_name == "-d") range = nOneInstance::e_range_maindir;
	}
	
	cout << "Will test; Application name is: " << application_name << endl;
	cout << "Run program with argument -h to see other options" << endl; // [option_h]
	cout << endl;

	nOneInstance::cInstanceObject instanceObject( range , application_name);
	bool i_am_only_instance = instanceObject.BeTheOnlyInstance();
	if (i_am_only_instance) {
		cout<<endl<<"The MAIN (SINGLE) instance: pid="<<getpid()<<"."<<endl;
		bool should_end=false;
		while (!should_end) {
			cout<<"Commands:"<<endl; 
			cout<<"  q = quit"<<endl;
			cout<<"  h = hang"<<endl;
			cout<<"  u = unhang (continue)"<<endl;
			std::string s; std::getline(std::cin, s);
			if (s=="q") should_end=true;
			if (s=="h") instanceObject.HangPings(true);
			if (s=="u") instanceObject.HangPings(false);
		}
	} else {
		_info("Other instance running, I will exit");
	}

	_info("Will exit main now");

}

