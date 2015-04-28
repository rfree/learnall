#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#define _info(X) do { std::cerr<<getpid()<<" "<<X<<std::endl; } while(0)
void Sleep(int ms) { 	std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

int main() {
	boost::interprocess::permissions perms;
	boost::interprocess::named_mutex mut(boost::interprocess::open_or_create, 
//	"instancegroup_NEWTEST_rafal28_user_n_l5_rafal_dir_ANY_instM1_PING3"

	"instancegroup_rafal29_user_n_l5_rafal_dir_ANY_instM1_PING" // <-- broken semaphore?

	//"instancegroup_rafal28_user_n_l5_rafal_dir_ANY_instM1_PING__________not_used_name"
	, perms );
	bool master=0;
	#if 0
	boost::interprocess::named_mutex leader_mut(boost::interprocess::open_or_create, "instancegroup_NEWTEST_rafal28_user_n_l5_rafal_dir_ANY_instM13" , perms );
	bool locked_leader = leader_mut.try_lock();
	if (locked_leader) { _info("Locked LEADER"); } else _info("Can not lock leader");
	master=locked_leader;
	#endif

	_info("loop");
	while(1) {
		Sleep(500);
		if (master) {
			_info("Unlocking");
			mut.unlock();
		} else {
			bool relock = mut.try_lock();	_info("relock="<<relock);
			if (relock) _info("RELOCKED WTF?!");
		}
	}
}

