#include <memory>

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

#include <thread>
#include <atomic>

#include <cstdio>
#include <cstdlib>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <unistd.h>
#include <pwd.h>

#include "liboneinstance.hpp"


// minor version (implementation) - number
#define nOneInstance_library_version_minor 2
// minor version (implementation) - number - is it WIP (or else it is a release)
#define nOneInstance_library_version_minor_wip 1


#define _info(X) do { std::cerr<<getpid()<<"/"<<(std::this_thread::get_id())<<" "<<X<<std::endl; } while(0)


namespace nOneInstance {

std::string GetLibraryVersionFull() {
	return "TODO"; // TODO
}



cNamedMutex::cNamedMutex(boost::interprocess::create_only_t, const char * name, const boost::interprocess::permissions & permissions)
: t_boost_named_mutex(boost::interprocess::create_only_t(), name, permissions), m_name(name), m_own(false)
{ }

cNamedMutex::cNamedMutex(boost::interprocess::open_or_create_t, const char * name, const boost::interprocess::permissions & permissions)
: t_boost_named_mutex(boost::interprocess::open_or_create_t(), name, permissions), m_name(name), m_own(false)
{ }

cNamedMutex::cNamedMutex(boost::interprocess::open_only_t, const char * name) 
: t_boost_named_mutex(boost::interprocess::open_only_t(), name), m_name(name), m_own(false)
{ }

cNamedMutex::~cNamedMutex() {
	_info("Destructor of named mutex in this="<<this<<" m_own="<<m_own<<" m_name="<<m_name);
	if (m_own) {
		_info("UNLOCKING on exit for m_name="<<m_name);
		this->unlock();
	}
}

std::string cNamedMutex::GetName() const {
	return m_name;
}

void cNamedMutex::SetOwnership(bool own) {
	m_own = own;
}

std::string cNamedMutex::EscapeMutexName(const std::string in) {
	// TODO lambda/algorithm
	std::string out;
	for (unsigned char c:in) {
		if (  ((c>='a')&&(c<='z')) || ((c>='0')&&(c<='9')) ) out += std::string(1,c);
		else {
			std::ostringstream oss;
			oss<<'_'<<std::hex<<((int)c);
			out += oss.str();
		}
	}
	return out;
}

std::string cNamedMutex::EscapeMutexNameWithLen(const std::string in) {
	std::ostringstream oss;
	oss << "l" << in.length() << "_" << EscapeMutexName(in);
	return oss.str();
}



cInstancePingable::cInstancePingable(const std::string &base_mutex_name, const boost::interprocess::permissions & mutex_perms)
: m_run_flag(false), m_go_flag(false),
m_mutex_name(BaseNameToPingName(base_mutex_name)), m_mutex_perms(mutex_perms)
{ 
	_info("Constructed cInstancePingable for m_mutex_name="<<m_mutex_name);
}


cInstancePingable::~cInstancePingable() {
	m_run_flag=false; // signall everyone, including the thread that it should stop

	if (m_thread) { // if thread runs (not null)
		_info("Joining the thread");
		m_thread->join(); 
		_info("Joining the thread - done");
	}

}

std::string cInstancePingable::BaseNameToPingName(const std::string &base_mutex_name) {
	std::string a = base_mutex_name;
	a += "_PING";
	_info("PING NAME will be: a="<<a);
	return a;
}

void cInstancePingable::PongLoop() {

	while (!m_go_flag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // sleep	TODO config
	}
	
	_info("PongLoop - begin for m_mutex_name="<<m_mutex_name);
	const int recreate_trigger = 10; // how often to recreate the object
	int need_recreate = recreate_trigger; // should we (re)create the mutex object now - counter. start at high level to create it initially
	while (m_run_flag) {
		_info("pong loop...");

		// create object if needed
		++need_recreate;
		if (need_recreate >= recreate_trigger) {
			_info("pong: CREATING the object now for m_mutex_name="<<m_mutex_name);
			_info("*****   m_mutex_name.c_str()                  ="<<m_mutex_name.c_str() );
			_info("pong: CREATING the object now for m_mutex_name="<<m_mutex_name);
			_info("*****   m_mutex_name.c_str()                  ="<<m_mutex_name.c_str() );
			_info("pong: CREATING the object now for m_mutex_name="<<m_mutex_name);
			_info("*****   m_mutex_name.c_str()                  ="<<m_mutex_name.c_str() );
			m_pong_obj.reset( 
				new cNamedMutex ( boost::interprocess::open_or_create, m_mutex_name.c_str(), m_mutex_perms ) 
			);
			_info("pong: CREATED object has name: " << m_pong_obj->GetName() );

			cNamedMutex test( boost::interprocess::open_or_create, "abc", m_mutex_perms );
			_info("test abc: " << test.GetName());

			m_pong_obj.reset( 
				new cNamedMutex( boost::interprocess::open_or_create, "abc", m_mutex_perms )
			);

			_info("reset test abc: " << m_pong_obj->GetName());
			_info("");
			_info("");
			_info("");

			need_recreate=false;
		}

		try {
			_info("PONG - unlocking, UNLOCKING "<<m_pong_obj->GetName());
			m_pong_obj->unlock(); ///< this signals that we are alive <--- ***
		} catch(...) { 
			_info("WARNING: unlocking - exception: need to re-create the object probably");
			need_recreate = recreate_trigger; 
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // sleep	TODO config
	}

	_info("PongLoop - end");
}

void cInstancePingable::Run() {
	if (m_run_flag) throw std::runtime_error("Can not Run already running object");
	_info("Running pong reply for m_mutex_name="<<m_mutex_name<<" - starting");
	m_run_flag=true; // allow it to run
	m_thread.reset( new std::thread(  [this]{ this->PongLoop(); }     ) ); // start the thread and start pong-loop inside it
	_info("Running pong reply for m_mutex_name="<<m_mutex_name<<" - done");
	m_go_flag=true; // fire it up now. we will not touch any data here
}






cInstanceObject::cInstanceObject(t_instance_range range, const std::string &program_name)
: m_range(range), m_program_name(program_name)
{ }

cInstanceObject::~cInstanceObject() {
	_info("Destructor of cInstanceObject at this="<<this);
}

bool cInstanceObject::BeTheOnlyInstance() {
	t_instance_outcome outcome = TryToBecomeInstance(1);
	if (outcome==e_instance_i_won) return true;
	return false;
}

bool cInstanceObject::PingInstance( const std::string &base_name, const boost::interprocess::permissions &permissions) {
	_info("Will ping name="<<base_name);
	int confidence=0; // are we sure no one is replying
	const int threshold = 5; // TODO config how sure we must be

	std::unique_ptr< cNamedMutex > ping_mutex( 
		new cNamedMutex(
			boost::interprocess::open_or_create,
			cInstancePingable::BaseNameToPingName( base_name ).c_str() , // the PONG name
			permissions
		)
	);

	_info("Locking the ping for first time");
	ping_mutex->timed_lock(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5));
	
	// lock it once (so it waits for open spot to request ping) - unless it's stucked for very long
	// because if it's very long then the instance hanged (and someone else locked it now or previously) 
	// so to await infinite wait

	long int pings=0;

	while (confidence < threshold) {
		bool relocked = false; 
		try {
			++pings;
			relocked = ping_mutex->try_lock();
		} catch(...) { } 
		if (relocked) {
			_info("Ping worked!");
			// TODO or the cleanup process recreated empty lock? better test once more
			return true; // the instance is alive!
		}
		_info("No-reply...");
		confidence++; // seems dead
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // sleep	TODO config
	}
	_info("Instance seems dead after pings="<<pings);

	return false;
}

t_instance_outcome cInstanceObject::TryToBecomeInstance(int inst) { // test instance, tries to become it
	_info("Trying instance inst="<<inst);

	const std::string igrp_name = std::string("instancegroup_") 
		+ m_program_name // testprogram
		+ ( (m_range==e_range_user) ? ("_user_n_"+cNamedMutex::EscapeMutexNameWithLen(GetUserName())) : ( "_user_ANY" ) ) // testprogram_alice  testprogram
		+ ( (m_range==e_range_maindir) ? ("_dir_n_"+cNamedMutex::EscapeMutexNameWithLen(GetDirName())) : ( "_dir_ANY" )   )
	;

	_info("igrp_name=" << igrp_name);

	boost::interprocess::permissions mutex_curr_perms;
	if (m_range==e_range_system) mutex_curr_perms.set_unrestricted(); // others users on same system need to synchronize with this

	std::ostringstream mutex_name_str; mutex_name_str << igrp_name << "_instM" << inst;
	std::string mutex_name = mutex_name_str.str(); // TODO move optimize?

	m_curr_mutex.reset( new cNamedMutex ( boost::interprocess::open_or_create, mutex_name.c_str(), mutex_curr_perms ) );
	// we created this mutex or it existed

	bool curr_locked = m_curr_mutex->try_lock();
	if (curr_locked) { // we locked it! 
		_info("Created and locked the Mc - we became the instance at inst="<<inst);
		m_curr_mutex->SetOwnership(true); // I own this mutex e.g. M5

		// I am probably the leader
		
		// ... TODO ... ping all lower numbers are they responding after all to ping ...


		_info("I will start responding to pings (curr)");
		m_pingable_curr.reset(new cInstancePingable( mutex_name, mutex_curr_perms )); // start responding to ping
		m_pingable_curr->Run();

		return e_instance_i_won; // we created and locked (or just locked - reclaimed) an instance
	}

	_info("Can not create/lock the Mc so we lost or it is dead. inst="<<inst);
	bool other_is_alive = PingInstance(mutex_name, mutex_curr_perms);

	if (! other_is_alive) {
		_info("The other instance is dead!");
		return e_instance_seems_dead;
	}
	
	return e_instance_i_lost;
}

std::string cInstanceObject::GetUserName() const {
  uid_t uid = geteuid ();
	struct passwd *pw = getpwuid (uid);
  if (pw) {
  	return pw->pw_name; // TODO escape characters
	}
	return "UNKNOWN";
}

std::string cInstanceObject::GetDirName() const {
	const size_t maxlen = 8192;
	char buf[maxlen];
	char * s1 = getcwd(buf,maxlen);
	if (s1) return s1;
	return "UNKNOWN";
}

} // namespace


