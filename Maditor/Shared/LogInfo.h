#pragma once

#include <boost\interprocess\containers\deque.hpp>
#include "SharedBase.h"

enum MessageType {
	ERROR_TYPE,
	WARNING_TYPE,
	LOG_TYPE
};

struct Traceback {

	char mFile[1024];
	char mFunction[512];
	int32_t mLineNr;
};

struct LogMessage {
	LogMessage(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mMsg(SharedCharAllocator(mgr)),
		mFullTraceback(SharedCharAllocator(mgr)) {}


	MessageType mType;
	SharedString mMsg;
	Traceback mTraceback;
	SharedString mFullTraceback;
};

typedef boost::interprocess::allocator<LogMessage,
	boost::interprocess::managed_shared_memory::segment_manager> SharedLogMessageAllocator;

typedef boost::interprocess::deque<LogMessage, SharedLogMessageAllocator> SharedLogMessageDeque;

class SharedLogMessageQueue : public std::queue<LogMessage, SharedLogMessageDeque> {
public:
	SharedLogMessageQueue(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		queue(mgr),
		mBuffer(mgr) {}

	LogMessage &create() {
		return mBuffer;
	}

	void send() {
		push(mBuffer);
	}

private:
	//boost::interprocess::managed_shared_memory::segment_manager *mMgr;
	LogMessage mBuffer;
};

struct SharedLog {
	SharedLog(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mMadgineLog(mgr) {}

	SharedLogMessageQueue mMadgineLog;
};