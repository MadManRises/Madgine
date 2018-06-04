#pragma once

#define BOOST_USE_WINDOWS_H
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/smart_ptr/shared_ptr.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/containers/deque.hpp>
#undef NO_ERROR

namespace Maditor {
	namespace Shared {

		typedef boost::interprocess::allocator<char,
			boost::interprocess::managed_shared_memory::segment_manager> SharedCharAllocator;
		typedef boost::interprocess::basic_string<char, std::char_traits<char>, SharedCharAllocator> SharedString;

	}
}
