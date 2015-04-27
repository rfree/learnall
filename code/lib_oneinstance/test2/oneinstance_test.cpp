#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

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
	cNamedMutex mutex(open_or_create, "fstream_named_mutex");	
	mutex.unlock();
	cout << mutex.try_lock() << endl;
	cout << mutex.try_lock() << endl;
	cout << mutex.try_lock() << endl;
}

int main(int argc, char **argv) {
	// test2(); return 0;

	using namespace std;
	vector<string> args; args.reserve(argc); for (int i=0; i<argc; ++i) args.push_back(argv[i]);

	if (argc>=2) if (args.at(1)=="-h") { ShowUsage(); return 0; }
	if (argc>=2) if (args.at(1)=="-v") { ShowTestProgramVersion(); return 0; }

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
		std::cout<<"Press ENTER to end the MAIN (SINGLE) instance: pid="<<getpid()<<"."<<std::endl;
		std::string s; std::getline(std::cin, s);
	} else {
		_info("Other instance running, I will exit");
	}
	_info("Will exit main now");
}

