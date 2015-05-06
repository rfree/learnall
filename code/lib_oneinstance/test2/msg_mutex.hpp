/*
 *  Created on: Apr 29, 2015
 *      Author: robert, rafal
 */

#ifndef CSAFEMUTEX_H_
#define CSAFEMUTEX_H_

#include <string>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/permissions.hpp>

class warning_already_unlocked : public std::exception { };

// http://lists.boost.org/boost-users/2012/03/73888.php
class msg_mutex
{
public:

	typedef char t_msg_char; // one character of the message
	typedef std::vector< t_msg_char > t_msg; // the message

	static constexpr int msglen_default = 8192;
	static constexpr char msgtxt_default = 'L'; // empty message to be used in lock
	// enum { msglen_default = 8192 };

	msg_mutex(const char* name, size_t msglen=msglen_default);

	msg_mutex(boost::interprocess::create_only_t create_only, const char *name,
			const boost::interprocess::permissions &perm = boost::interprocess::permissions(), size_t msglen=msglen_default);
	msg_mutex(boost::interprocess::open_or_create_t open_or_create, const char *name,
			const boost::interprocess::permissions &perm = boost::interprocess::permissions(), size_t msglen=msglen_default);
	msg_mutex(boost::interprocess::open_only_t open_only, const char *name, size_t msglen=msglen_default);

	~msg_mutex();

	bool remove(); // TODO also a static member function for this
	std::string get_name();

	// mutex-like api:
	void lock(); // lock mutex, block thread if mutex is locked
	void unlock(); // safe unlock, if mutex is unlocked does nothing
	bool try_lock(); // tries to lock the mutex, returns false when the mutex is already locked, returns true when success
	bool try_unlock();
	bool timed_lock(const boost::posix_time::seconds &sec); // return true when locks

	bool is_locked();
	
	// message-like api:
	void lock_msg(const t_msg& msg); // lock mutex, block thread if mutex is locked
	t_msg unlock_msg(); // safe unlock, if mutex is unlocked does nothing
	bool try_lock_msg(const t_msg& msg); // tries to lock the mutex, returns false when the mutex is already locked, returns true when success
	t_msg try_unlock_msg();
	bool timed_lock_msg(const boost::posix_time::seconds &sec, const t_msg& msg); // return true when locks

private:
	const std::string mName;
	boost::interprocess::message_queue mMsgQueue;
	int mBuffer;
};

#endif /* CSAFEMUTEX_H_ */

