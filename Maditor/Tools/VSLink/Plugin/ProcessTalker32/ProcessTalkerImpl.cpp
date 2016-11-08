

#include "ProcessTalkerImpl.h"

namespace ProjectTalker {

	ProcessTalkerImpl::ProcessTalkerImpl(const std::string & processId, const std::string & name, MessageReceiver receiver) :
		ProcessTalker(processId, name),
		mReceiver(receiver)
	{
	}


	void ProcessTalkerImpl::receiveMessage(const Message &msg)
	{
		mReceiver(msg);
	}

}