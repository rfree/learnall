#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <fstream>
#include <memory>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#define _info(X) do { std::cerr<<getpid()<<" "<<X<<std::endl; } while(0)

namespace nOneInstance {

typedef enum { e_range_system=100, e_range_user, e_range_maindir } t_instance_range;

using std::string;

class cInstanceObject {
	protected:
		std::unique_ptr< boost::interprocess::named_mutex > m_main_mutex;

	public:
		cInstanceObject()=default; ///< default constructor
		virtual ~cInstanceObject();

		cInstanceObject(const cInstanceObject &)=delete; // do not copy!
		cInstanceObject& operator=(const cInstanceObject &)=delete; // do not copy!
		cInstanceObject& operator=(cInstanceObject &&)=delete; // do not copy!

		bool BeTheOnlyInstance(const t_instance_range range);
};

cInstanceObject::~cInstanceObject() {
	m_main_mutex->unlock();
}

bool cInstanceObject::BeTheOnlyInstance(const t_instance_range range) {
	_info("Started");

	const string user_name = "foo" ; // TODO
	const string maindir_name = "currdir"; // TODO

	const string program_name = "test_program2";
	const string igrp_name = std::string("instancegroup_") 
		+ program_name // testprogram
		+ ( (range==e_range_user) ? ("_user_"+user_name) : ( "_user_ANY" ) ) // testprogram_alice  testprogram
		+ ( (range==e_range_maindir) ? ("_dir_"+maindir_name) : ( "_dir_ANY" )   )
	;

	_info("igrp_name=" << igrp_name);

	boost::interprocess::permissions mutex1_perms;
	if (range==e_range_system) mutex1_perms.set_unrestricted(); // others users on same system need to synchronize with this

	m_main_mutex.reset(
		new boost::interprocess::named_mutex( boost::interprocess::open_or_create, (igrp_name+"_mutex1").c_str() , mutex1_perms )
	);

	bool main_mutex_locked = m_main_mutex->try_lock();
	if (main_mutex_locked) {
		_info("Created and locked the MM");
		return true;
	}

	return false;
}

} // namespace

int main() {
	nOneInstance::cInstanceObject instanceObject; // automatic storage duration / local
	bool i_am_only_instance = instanceObject.BeTheOnlyInstance(nOneInstance::e_range_user);
	if (i_am_only_instance) {
		std::cout<<"Press ENTER to end the MAIN (SINGLE) instance: pid="<<getpid()<<"."<<std::endl;
		std::string s; std::getline(std::cin, s);
	} else {
		_info("Other instance running, I will exit");
	}
	_info("Will exit main now");
}

