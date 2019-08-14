#pragma once

#include "Interfaces/debug/stacktrace.h"

namespace Engine
{
	namespace Util
	{

		class LogListener
		{
		public:
			virtual ~LogListener() = default;
			virtual void messageLogged(const std::string & message, Engine::Util::MessageType lml, const Engine::Debug::StackTrace<32> &stackTrace, const std::string & logName) = 0;
		};
		
	}
}
