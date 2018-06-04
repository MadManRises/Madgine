#pragma once

#include <string>


namespace boost {
	namespace interprocess {

		template < class T, class DifferenceType = int32_t
			, class OffsetType = uint64_t, std::size_t Alignment = 0>
			class offset_ptr;

		template<class VoidPointer>
		class message_queue_t;

		typedef message_queue_t<offset_ptr<void> > message_queue;



	}
}


#define BOOST_DATE_TIME_NO_LIB
#include <boost\interprocess\ipc\message_queue.hpp>
#include <boost\interprocess\mapped_region.hpp>



template <class T>
class ProcessTalker {
public:
	ProcessTalker(const std::string &processId, const std::string &name) :
		mFullName(processId + "-" + name),
		mProcessId(processId),
		mReadQueue((boost::interprocess::message_queue::remove(mFullName.c_str()), boost::interprocess::create_only), mFullName.c_str(), 100, sizeof(T))
	{
	}

	~ProcessTalker()
	{
		boost::interprocess::message_queue::remove(mFullName.c_str());
	}

	void update()
	{
		boost::interprocess::message_queue::size_type size;
		unsigned int priority;
		while (mReadQueue.try_receive(&mBuffer, sizeof(T), size, priority)) {
			assert(size == sizeof(mBuffer));
			receiveMessage(mBuffer);
		}
	}


	bool sendMsg(const T &msg, const std::string &target)
	{
		std::string path = mProcessId + "-" + target;
		try {
			boost::interprocess::message_queue queue(boost::interprocess::open_only, path.c_str());
			return queue.try_send(&msg, sizeof(T), 0);
		}
		catch (boost::interprocess::interprocess_exception &) {
			return false;
		}
	}
	virtual void receiveMessage(const T &msg) = 0;

private:

	T mBuffer;

	std::string mFullName;
	std::string mProcessId;
	boost::interprocess::message_queue mReadQueue;

};