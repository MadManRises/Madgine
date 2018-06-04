#pragma once

#include "ProcessTalkerImpl.h"
#include "..\..\..\ProcessTalker.h"

namespace ProjectTalker {

	
	class pImpl : public ProcessTalker<VSMsg> {
	public:
		pImpl(const std::string &processId, const std::string &name, MessageReceiver receiver);

		// Inherited via ProcessTalker
		virtual void receiveMessage(const VSMsg &msg);

	private:
		MessageReceiver mReceiver;
	};



}