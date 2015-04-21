#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <fstream>
#include <memory>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>

#define _info(X) do { std::cerr<<getpid()<<" "<<X<<std::endl; } while(0)

namespace nOneInstance {

// what type of instance do want, e.g. one per system, or per user, or per directory:
typedef enum { e_range_system=100, e_range_user, e_range_maindir } t_instance_range;

// what is the situation of given instance e.g. inst1, inst2, inst3 - it can be dead, or was not locked/existing 
// and we won the race to it, or we lost (we lost the race, or it was alive etc)
typedef enum { e_instance_i_won=50, e_instance_i_lost=60, e_instance_seems_dead=70 } t_instance_outcome;

using std::string;

typedef boost::interprocess::named_mutex t_boost_named_mutex;

// TODO C++11 constructor tag forwarding 
// boost::interprocess::named_mutex t_boost_named_mutex;
		
class cNamedMutex : public t_boost_named_mutex { // public boost::interprocess::named_mutex {
	private:
		bool m_own; ///< do I own this mutex, e.g. do I have the right to unlock it (when exiting)

	public:
	//	using t_boost_named_mutex::named_mutex; TODO use forwarding:
	//	using boost::interprocess::named_mutex::named_mutex; ///< forward ALL the constructors o/ http://stackoverflow.com/questions/3119929/forwarding-all-constructors-in-c0x

		cNamedMutex(boost::interprocess::create_only_t, const char * name, const boost::interprocess::permissions & = boost::interprocess::permissions());
		cNamedMutex(boost::interprocess::open_or_create_t, const char * name, const boost::interprocess::permissions & = boost::interprocess::permissions());
		cNamedMutex(boost::interprocess::open_only_t, const char * name);

		~cNamedMutex();

		void SetOwnership(bool own=true); ///< set ownership (does not clear/unlock if we disown object!)
		static std::string EscapeMutexName(const std::string in); ///< escape any string so it becames a valid name (and part of name) of named-mutex object
		static std::string EscapeMutexNameWithLen(const std::string in); ///< escape also, but prepends the length of string to make this immune to any corner-case bugs
};

cNamedMutex::cNamedMutex(boost::interprocess::create_only_t, const char * name, const boost::interprocess::permissions & permissions)
: t_boost_named_mutex(boost::interprocess::create_only_t(), name, permissions), m_own(false)
{ }

cNamedMutex::cNamedMutex(boost::interprocess::open_or_create_t, const char * name, const boost::interprocess::permissions & permissions)
: t_boost_named_mutex(boost::interprocess::open_or_create_t(), name, permissions), m_own(false)
{ }

cNamedMutex::cNamedMutex(boost::interprocess::open_only_t, const char * name) 
: t_boost_named_mutex(boost::interprocess::open_only_t(), name), m_own(false)
{ }

cNamedMutex::~cNamedMutex() {
	_info("Destructor of named mutex in this="<<this<<" m_own="<<m_own);
	if (m_own) {
		_info("Unlocking");
		this->unlock();
	}
}

std::string cNamedMutex::EscapeMutexName(const std::string in) {
	// TODO lambda/algorithm
	std::string out;
	for (unsigned char c:in) {
		if (  ((c>='a')&&(c<='z')) || ((c>='0')&&(c<='9')) ) out += std::string(1,c);
		else {
			std::ostringstream oss;
			oss<<'_'<<std::hex<<((int)c)<<std::ends;
			out += oss.str();
		}
	}
	return out;
}

std::string cNamedMutex::EscapeMutexNameWithLen(const std::string in) {
	std::ostringstream oss;
	oss << "l" << in.length() << "_" << EscapeMutexName(in) << std::ends;
	return oss.str();
}

void cNamedMutex::SetOwnership(bool own) {
	m_own = own;
}


class cInstanceObject {
	protected:
		const t_instance_range m_range; ///< range of this instance
		const std::string m_program_name; ///< a name for the program as assigned by developer so other instances will find you; NOT executable name/pid/std related.

		std::unique_ptr< cNamedMutex > m_curr_mutex; ///< access the current mutex (e.g. m3, m4... if m1 was taken)
		std::unique_ptr< cNamedMutex > m_m1_mutex; ///< access the m1 mutex (once we are sure we are the leader)

		t_instance_outcome TryToBecomeInstance(int inst); // test instance, tries to become it
		bool PingInstance(int inst); // tries to ping instance, is it alive

	public:
		cInstanceObject(t_instance_range range, const std::string &program_name);
		virtual ~cInstanceObject();

		cInstanceObject(const cInstanceObject &)=delete; // do not copy!
		cInstanceObject& operator=(const cInstanceObject &)=delete; // do not copy!
		cInstanceObject& operator=(cInstanceObject &&)=delete; // do not copy!

		bool BeTheOnlyInstance();

		std::string GetUserName() const;
		std::string GetDirName() const;
};

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

bool cInstanceObject::PingInstance(int inst) { // tries to ping instance, is it alive
}

t_instance_outcome cInstanceObject::TryToBecomeInstance(int inst) { // test instance, tries to become it
	_info("Trying instance inst="<<inst);

	const string igrp_name = std::string("instancegroup_") 
		+ m_program_name // testprogram
		+ ( (m_range==e_range_user) ? ("_user_n_"+cNamedMutex::EscapeMutexNameWithLen(GetUserName())) : ( "_user_ANY" ) ) // testprogram_alice  testprogram
		+ ( (m_range==e_range_maindir) ? ("_dir_n_"+cNamedMutex::EscapeMutexNameWithLen(GetDirName())) : ( "_dir_ANY" )   )
	;

	_info("igrp_name=" << igrp_name);

	boost::interprocess::permissions mutex1_perms;
	if (m_range==e_range_system) mutex1_perms.set_unrestricted(); // others users on same system need to synchronize with this

	std::ostringstream mutex_name_str; mutex_name_str << igrp_name << "_instM" << inst;
	string mutex_name = mutex_name_str.str(); // TODO move optimize?

	m_curr_mutex.reset( new cNamedMutex ( boost::interprocess::open_or_create, mutex_name.c_str(), mutex1_perms ) );
	// we created this mutex or it existed

	bool curr_locked = m_curr_mutex->try_lock();
	if (curr_locked) {
		_info("Created and locked the Mc - we became the instance at inst="<<inst);
		m_curr_mutex->SetOwnership(true); // I own this mutex e.g. M5
		return e_instance_i_won; // we created and locked (or just locked - reclaimed) an instance
	}

	_info("Can not create/lock the Mc so we lost at inst="<<inst);
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

int main() {
	nOneInstance::cInstanceObject instanceObject( nOneInstance::e_range_maindir , "programrafal"); // automatic storage duration / local
	bool i_am_only_instance = instanceObject.BeTheOnlyInstance();
	if (i_am_only_instance) {
		std::cout<<"Press ENTER to end the MAIN (SINGLE) instance: pid="<<getpid()<<"."<<std::endl;
		std::string s; std::getline(std::cin, s);
	} else {
		_info("Other instance running, I will exit");
	}
	_info("Will exit main now");
}


#if 0

struct p { };
struct q { };

struct A { 
	A(p) { }
	A(q) { }
};

p pp;
q qq;

struct B : A { 
	B(p) : A(pp) { }
	B(q) : A(qq) { }
};

int main() { 
	A a1(p);
	A a1(q);
}



#endif