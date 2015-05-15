#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <chrono>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <exception>
#include <sys/types.h>
#include <unistd.h>

using namespace boost::interprocess;

void work()
{
	std::cout << "work" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(3));
}
/*************************************************************/
/*************************************************************/
/*************************************************************/
class c_one_instance
{
	public:
		c_one_instance() = delete;
		c_one_instance(const c_one_instance &) = delete;
		c_one_instance(c_one_instance &&) = delete;
		c_one_instance & operator = (const c_one_instance &) = delete;
		c_one_instance & operator = (c_one_instance &&) = delete;

		c_one_instance(const std::string &instance_name);
		~c_one_instance();

	private:
		const std::string m_instance_name;
		std::atomic<bool> m_end_of_thread;
		std::unique_ptr<boost::interprocess::named_mutex> m_main_mutex;
		std::unique_ptr<boost::interprocess::named_mutex> m_ping_mutex;
		boost::interprocess::permissions m_perm;
		bool m_unlock_main_mutex;
		unsigned int m_my_number;
		std::unique_ptr<std::thread> m_ping_thread;

		void start_response_ping(const std::string &ping_mutex_name);
		bool ping(const unsigned int number_of_mutex);
		const std::string get_name_of_ping_mutex(const unsigned int ping_mutex_number);
		bool get_next_mutex();
};
/*************************************************************/
c_one_instance::c_one_instance(const std::string &instance_name)
:
m_instance_name(instance_name),
m_my_number(0),
m_unlock_main_mutex(false),
m_end_of_thread(false)
{
	std::string name_of_mutex = m_instance_name + std::string("_") + std::to_string(m_my_number);
	std::cout << "main mutex name: " << name_of_mutex << std::endl;
	std::cout << "main mutex name (c_str()) " << name_of_mutex.c_str() << std::endl;

	m_perm.set_unrestricted();
	m_main_mutex.reset(new boost::interprocess::named_mutex (open_or_create, name_of_mutex.c_str(), m_perm));

	if (m_main_mutex->try_lock() == false)
	//if (m_main_mutex->timed_lock(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(1)) == false)
	{
		std::cout << name_of_mutex << " is locked (try_lock == false)" << std::endl;
		std::cout << "try ping" << std::endl;
		if (ping(m_my_number) == true)
		{
			std::cout << "EXIT" << std::endl;
			throw std::runtime_error("Process exists");
		}
		else
		{

			std::cout << "start work" << std::endl;
		}
	}

	m_unlock_main_mutex = true;
	std::cout << "try_lock == true, I can work" << std::endl;
	start_response_ping(get_name_of_ping_mutex(m_my_number));
}
/*************************************************************/
c_one_instance::~c_one_instance()
{
	m_end_of_thread = true;
	//m_main_mutex.reset(nullptr);
	m_ping_thread->join();
	if (m_unlock_main_mutex)
	{
		m_main_mutex->unlock();
		std::cout << "unlock" << std::endl;
	}
	std::cout << "destructor" << std::endl;
}
/*************************************************************/
void c_one_instance::start_response_ping(const std::string &ping_mutex_name)
{
	//m_ping_mutex.reset(new boost::interprocess::named_mutex (open_or_create, ping_mutex_name.c_str()));
	//boost::interprocess::named_mutex ping_mutex(open_or_create, ping_mutex_name.c_str());
	//std::cout << "Strat response ping" << std::endl;
	//std::cout << "Ping mutex name " << ping_mutex_name << std::endl;
	m_ping_thread.reset(new std::thread ([this, ping_mutex_name]()
		{
			do
			{
				std::cout << "response ping on " << ping_mutex_name.c_str() << std::endl;
				boost::interprocess::named_mutex ping_mutex(open_or_create, ping_mutex_name.c_str(), m_perm);
				//m_ping_mutex->unlock();
				ping_mutex.unlock();
				std::this_thread::sleep_for(std::chrono::seconds(1)); // TODO sleep time
			}
			while(!m_end_of_thread);
			std::cout << "stop response ping" << std::endl;
		}));
	//ping_thread.detach();
}
/*************************************************************/
bool c_one_instance::ping(const unsigned int number_of_mutex)
{
	boost::interprocess::named_mutex remote_ping_mutex(open_or_create, get_name_of_ping_mutex(number_of_mutex).c_str(), m_perm);
	//std::cout << "ping remote process " << get_name_of_ping_mutex(number_of_mutex) << std::endl;
	std::cout << "ping mutex " << get_name_of_ping_mutex(number_of_mutex).c_str() << std::endl;
	/*remote_ping_mutex.try_lock();
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::cout << "wait for response" << std::endl;
	if (remote_ping_mutex.try_lock() == false)*/
	remote_ping_mutex.try_lock();
	if (remote_ping_mutex.timed_lock(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5)) == false)
	{
		std::cout << "ping: no response" << std::endl;
		return false;
	}
	else
	{
		std::cout << "ping: response" << std::endl;
		return true;
	}
}
/*************************************************************/
const std::string c_one_instance::get_name_of_ping_mutex(const unsigned int ping_mutex_number)
{
	return m_instance_name + std::string("_") + std::to_string(m_my_number) + std::string("_ping");
}
/*************************************************************/
bool c_one_instance::get_next_mutex()
{
	m_my_number++;
	std::string name_of_mutex = m_instance_name + std::string("_") + std::to_string(m_my_number);
	m_main_mutex.reset(new boost::interprocess::named_mutex (open_or_create, name_of_mutex.c_str(), m_perm));
	std::cout << "main mutex name (c_str()) " << name_of_mutex.c_str() << std::endl;
	std::cout << "try lock" << std::endl;
	if (m_main_mutex->try_lock() == false)
	{
		std::cout << name_of_mutex << " is locked (try_lock == false)" << std::endl;
		std::cout << "try ping" << std::endl;
		if (ping(m_my_number) == true)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}
/*************************************************************/
/*************************************************************/
/*************************************************************/
int main()
{
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != nullptr)
		std::cout << "Current working dir: " << cwd << std::endl;
	else
		std::cout << "getcwd() error" << std::endl;

	std::string instance_name(cwd);
	std::string non_alpha;
	std::replace_if(instance_name.begin(), instance_name.end(),
			[&non_alpha](char c)
			{
				if (std::isalpha(c))
				{
					return false;
				}
				else
				{
					non_alpha += c;
					return true;
				}
			}
		, '_');

	while (!non_alpha.empty())
	{
		instance_name += std::to_string(static_cast<int>(non_alpha.front()));
		non_alpha.erase(non_alpha.begin());
	}

	std::cout << "instance_name " << instance_name << std::endl;
	std::cout << "non_alpha " << non_alpha << std::endl;

	//c_one_instance instance("my_instance");
	c_one_instance instance(instance_name);
	char a;
	std::cin >> a;

	return 0;
}

//g++ -std=c++11 src/one_instance.cpp -o one_instance -lboost_system
