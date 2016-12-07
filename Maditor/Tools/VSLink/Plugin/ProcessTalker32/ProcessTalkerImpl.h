#pragma once


#include "..\..\VSMsg.h"

namespace ProjectTalker {

	class pImpl;

	typedef void(__stdcall *MessageReceiver)(const VSMsg &msg);

	class ProcessTalkerImpl {
	public:
		ProcessTalkerImpl(const std::string &processId, const std::string &name, MessageReceiver receiver);
		virtual ~ProcessTalkerImpl();

		void run(bool *b);
		bool sendMsg(const VSMsg &msg, const std::string &target);

	private:
		pImpl *impl;		
	};



}