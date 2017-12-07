#pragma once

namespace Engine
{
	namespace Util
	{

		class Log
		{
		public:
			virtual ~Log() = default;
			virtual void log(const std::string& msg, MessageType lvl, const std::list<TraceBack>& traceBack = {}) = 0;
			virtual std::string getName() = 0;
		};
		
	}
}
