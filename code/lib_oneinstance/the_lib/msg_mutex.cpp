/*
 *  Created on: Apr 29, 2015
 *      Author: robert, rafal
 */

#include "msg_mutex.hpp"

#include <thread>

#ifndef _info
	#define _info(X) do { std::cerr<<" "<<__FILE__<<getpid()<<"/"<<(std::this_thread::get_id())<<" "<<X<<std::endl; } while(0)
#endif

msg_mutex::msg_mutex(const char * name, size_t msglen)
	:
	mName(name),
	m_msglen(1),
	mMsgQueue(boost::interprocess::open_or_create, name, 1, m_msglen),
	mBuffer(m_msglen)
{
	_info("Constructed this="<<this<<" msglen="<<msglen<<" name="<<name);
}

msg_mutex::msg_mutex(boost::interprocess::create_only_t create_only,
		const char *name,
		const boost::interprocess::permissions &perm, size_t msglen)
:
	mName(name),
	m_msglen(msglen),
	mMsgQueue(create_only, name, 1, m_msglen, perm),
	mBuffer(m_msglen)
{
}

msg_mutex::msg_mutex(boost::interprocess::open_or_create_t open_or_create,
		const char *name,
		const boost::interprocess::permissions &perm, size_t msglen)
:
	mName(name),
	m_msglen(msglen),
	mMsgQueue(open_or_create, name, 1, m_msglen, perm),
	mBuffer(m_msglen)
{
}

msg_mutex::msg_mutex(boost::interprocess::open_only_t open_only, const char *name, size_t msglen)
:
	mName(name),
	m_msglen(msglen),
	mMsgQueue(open_only, name),
	mBuffer(0)
{
}

msg_mutex::~msg_mutex()
{
}

void msg_mutex::lock() {
	const char * ptr_ch = & msgtxt_default;
	const void * ptr_void = ptr_ch;
	mMsgQueue.send(ptr_void, sizeof(msgtxt_default), 0);
}

void msg_mutex::lock_msg(const t_msg& msg) {
	mMsgQueue.send( msg.data() , sizeof(msg.at(0)) * msg.size(), 0);
}

bool msg_mutex::try_lock() {
	return mMsgQueue.try_send( mBuffer.data(), sizeof(int), 0);
}

void msg_mutex::unlock() {
	boost::interprocess::message_queue::size_type recvd_size;
	unsigned int priority;
	if (!mMsgQueue.try_receive(&mBuffer, mBuffer.size()*sizeof(mBuffer.at(0)), recvd_size, priority))	{
		throw warning_already_unlocked();
	}
}

msg_mutex::t_msg msg_mutex::unlock_msg() {
	t_msg buff;
	buff.resize(m_msglen);
	boost::interprocess::message_queue::size_type recvd_size;
	unsigned int priority;
	if (!mMsgQueue.try_receive( buff.data(), sizeof(t_msg_char)*m_msglen, recvd_size, priority))	{
		throw warning_already_unlocked();
	}
	buff.resize(recvd_size); // cutt off the not used data
	return buff;
}

bool msg_mutex::try_unlock() {
	try {
		unlock();
		return true;
	}
	catch (...) {	return false;	}
}

bool msg_mutex::is_locked() {
	if (mMsgQueue.get_num_msg() == 1) {	return true; }
	else { return false; }
}

bool msg_mutex::timed_lock(const boost::posix_time::seconds &sec) {
	return mMsgQueue.timed_send(&mBuffer, sizeof(int), 0, boost::posix_time::second_clock::universal_time() + sec);
}

bool msg_mutex::remove() {
	return boost::interprocess::message_queue::remove(mName.c_str());
}

std::string msg_mutex::get_name() {
	return mName;
}


char msg_mutex::msgtxt_default = 'L'; // empty message to be used in lock


#undef _info

