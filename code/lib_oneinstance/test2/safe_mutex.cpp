/*
 * CSafeMutex.cpp
 *
 *  Created on: Apr 29, 2015
 *      Author: robert
 */

#include "CSafeMutex.hpp"

CSafeMutex::CSafeMutex(const std::string &name)
:
mName(name),
mMsgQueue(boost::interprocess::open_or_create, name.c_str(), 1, sizeof(int)),
mBuffer(0)
{
}

CSafeMutex::~CSafeMutex()
{
}

void CSafeMutex::lock()
{
	mMsgQueue.send(&mBuffer, sizeof(int), 0);
}

void CSafeMutex::unlock()
{
	int buff;
	boost::interprocess::message_queue::size_type recvd_size;
	unsigned int priority;
	if (!mMsgQueue.try_receive(&buff, sizeof(int), recvd_size, priority))
	{
		throw std::runtime_error("Unlock unlocked mutex");
	}
}

bool CSafeMutex::try_unlock()
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

bool CSafeMutex::is_locked()
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

bool CSafeMutex::try_lock()
{
	return mMsgQueue.try_send(&mBuffer, sizeof(int), 0);
}

bool CSafeMutex::timed_lock(const boost::posix_time::seconds &sec)
{
	return mMsgQueue.timed_send(&mBuffer, sizeof(int), 0, boost::posix_time::second_clock::universal_time() + sec);
}

bool CSafeMutex::remove()
{
	return boost::interprocess::message_queue::remove(mName.c_str());
}

std::string CSafeMutex::get_name()
{
	return mName;
}
