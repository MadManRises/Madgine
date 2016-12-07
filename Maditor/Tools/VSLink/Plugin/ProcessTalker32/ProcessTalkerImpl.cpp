

#include "ProcessTalkerImpl.h"
#include "impl.h"

namespace ProjectTalker {

	ProcessTalkerImpl::ProcessTalkerImpl(const std::string & processId, const std::string & name, MessageReceiver receiver) :
		impl(new pImpl(processId, name, receiver))
	{
	}

	ProcessTalkerImpl::~ProcessTalkerImpl()
	{
		delete impl;
	}

	void ProcessTalkerImpl::run(bool * b)
	{
		while (*b) {
			impl->update();
		}
	}

	bool ProcessTalkerImpl::sendMsg(const VSMsg & msg, const std::string & target)
	{
		return impl->sendMsg(msg, target);
	}

}