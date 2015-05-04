/*
 *  Created on: Apr 29, 2015
 *      Author: robert
 */

#ifndef CSAFEMUTEX_H_
#define CSAFEMUTEX_H_

#include <string>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// http://lists.boost.org/boost-users/2012/03/73888.php
class safe_mutex
{
public:
	safe_mutex(const std::string &name);
	~safe_mutex(); // unlock mutex

	void lock(); // lock mutex, block thread if mutex is locked
	void unlock(); // safe unlock, if mutex is unlocked does nothing
	bool try_unlock();
	bool is_locked();
	bool try_lock(); // tries to lock the mutex, returns false when the mutex is already locked, returns true when success
	bool timed_lock(const boost::posix_time::seconds &sec); // return true when locks
	bool remove();

	std::string get_name();

private:
	const std::string mName;
	boost::interprocess::message_queue mMsgQueue;
	int mBuffer;
};

#endif /* CSAFEMUTEX_H_ */

