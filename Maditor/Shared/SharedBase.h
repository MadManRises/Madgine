#pragma once

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/list.hpp>

typedef boost::interprocess::allocator<char,
	boost::interprocess::managed_shared_memory::segment_manager> SharedCharAllocator;
typedef boost::interprocess::basic_string<char, std::char_traits<char>, SharedCharAllocator> SharedString;





template <class T>
class SharedList : public boost::interprocess::list<T, boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>> {
public:
	SharedList(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		list(boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>(mgr))
	{

	}

	T &push() {
		emplace_back(SharedMemory::mgr());
		return back();
	}

};