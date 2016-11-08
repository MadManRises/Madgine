#pragma once

#include "ProcessTalker.h"

namespace ProjectTalker {

	typedef void(__stdcall *MessageReceiver)(const Message &msg);

	class ProcessTalkerImpl : public ProcessTalker {
	public:
		ProcessTalkerImpl(const std::string &processId, const std::string &name, MessageReceiver receiver);

		// Inherited via ProcessTalker
		virtual void receiveMessage(const Message &msg);

	private:
		MessageReceiver mReceiver;
	};



}