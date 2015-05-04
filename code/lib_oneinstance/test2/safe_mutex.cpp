/*
 *  Created on: Apr 29, 2015
 *      Author: robert
 */

#include "safe_mutex.hpp"

safe_mutex::safe_mutex(const std::string &name)
:
mName(name),
mMsgQueue(boost::interprocess::open_or_create, name.c_str(), 1, sizeof(int)),
mBuffer(0)
{
}

safe_mutex::safe_mutex(boost::interprocess::create_only_t create_only,
		const char *name,
		const boost::interprocess::permissions &perm)
:
mName(name),
mMsgQueue(create_only, name, 1, sizeof(int), perm),
mBuffer(0)
{
}

safe_mutex::safe_mutex(boost::interprocess::open_or_create_t open_or_create,
		const char *name,
		const boost::interprocess::permissions &perm)
:
mName(name),
mMsgQueue(open_or_create, name, 1, sizeof(int), perm),
mBuffer(0)
{
}

safe_mutex::safe_mutex(boost::interprocess::open_only_t open_only, const char *name)
:
mName(name),
mMsgQueue(open_only, name),
mBuffer(0)
{
}

safe_mutex::~safe_mutex()
{
}

void safe_mutex::lock()
{
	mMsgQueue.send(&mBuffer, sizeof(int), 0);
}

void safe_mutex::unlock()
{
	int buff;
	boost::interprocess::message_queue::size_type recvd_size;
	unsigned int priority;
	if (!mMsgQueue.try_receive(&buff, sizeof(int), recvd_size, priority))
	{
		throw warning_already_unlocked();
	}
}

bool safe_mutex::try_unlock()
{
	try
	{
		unlock();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool safe_mutex::is_locked()
{
	if (mMsgQueue.get_num_msg() == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool safe_mutex::try_lock()
{
	return mMsgQueue.try_send(&mBuffer, sizeof(int), 0);
}

bool safe_mutex::timed_lock(const boost::posix_time::seconds &sec)
{
	return mMsgQueue.timed_send(&mBuffer, sizeof(int), 0, boost::posix_time::second_clock::universal_time() + sec);
}

bool safe_mutex::remove()
{
	return boost::interprocess::message_queue::remove(mName.c_str());
}

std::string safe_mutex::get_name()
{
	return mName;
}

