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
	"ex1"
	, perms );

	const int t=1500;
	_info("unlock"); mut.unlock(); Sleep(t);
	_info("unlock"); mut.unlock(); Sleep(t);
	_info("unlock"); mut.unlock(); Sleep(t);

	_info("(try_) lock:   " << mut.try_lock() ); Sleep(t);
	_info("(try_) lock:   " << mut.try_lock() ); Sleep(t);
	_info("(try_) lock:   " << mut.try_lock() ); Sleep(t);
	_info("(try_) lock:   " << mut.try_lock() ); Sleep(t);
	_info("(try_) lock:   " << mut.try_lock() ); Sleep(t);
	_info("(try_) lock:   " << mut.try_lock() ); Sleep(t);
	_info("(try_) lock:   " << mut.try_lock() ); Sleep(t);
}

