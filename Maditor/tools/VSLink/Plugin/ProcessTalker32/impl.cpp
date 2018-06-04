

#include "impl.h"

namespace ProjectTalker {

	pImpl::pImpl(const std::string & processId, const std::string & name, MessageReceiver receiver) :
		ProcessTalker(processId, name),
		mReceiver(receiver)
	{
	}


	void pImpl::receiveMessage(const VSMsg &msg)
	{
		mReceiver(msg);
	}

}