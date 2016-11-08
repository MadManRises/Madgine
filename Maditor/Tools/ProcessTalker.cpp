#include "ProcessTalker.h"

#include <chrono>
#include <thread>

#define BOOST_DATE_TIME_NO_LIB
#include <boost\interprocess\ipc\message_queue.hpp>

ProcessTalker::ProcessTalker(const std::string & processId, const std::string & name) :
	mFullName(processId + "-" + name),
	mProcessId(processId),
	mReadQueue(0)
{
	message_queue::remove(mFullName.c_str());
	mReadQueue = new message_queue(create_only, mFullName.c_str(), 100, sizeof(mBuffer));
}

ProcessTalker::~ProcessTalker()
{
	message_queue::remove(mFullName.c_str());
	delete mReadQueue;
}

void ProcessTalker::update()
{
	message_queue::size_type size;
	unsigned int priority;
	while (mReadQueue->try_receive(&mBuffer, sizeof(mBuffer), size, priority)) {
		assert(size == sizeof(mBuffer));
		receiveMessage({ mBuffer.mType, mBuffer.mArg1, mBuffer.mArg2 });
	}
}

void ProcessTalker::run(bool *b)
{
	using namespace std::chrono_literals;
	while (*b) {
		update();
		std::this_thread::sleep_for(250ms);
	}
}

bool ProcessTalker::sendMsg(const Message & msg, const std::string & target)
{
	std::string path = mProcessId + "-" + target;
	try {
		message_queue queue(open_only, path.c_str());

		mBuffer.mType = msg.mType;
		assert(msg.mArg1.size() < sizeof(mBuffer.mArg1));
		strcpy(mBuffer.mArg1, msg.mArg1.c_str());
		mBuffer.mArg2 = msg.mArg2;
		return queue.try_send(&mBuffer, sizeof(mBuffer), 0);
	}
	catch (interprocess_exception &e) {
		return false;
	}
}
