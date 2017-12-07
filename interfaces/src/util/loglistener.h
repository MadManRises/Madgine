#pragma once

namespace Engine
{
	namespace Util
	{

		class LogListener
		{
		public:
			virtual ~LogListener() = default;
			virtual void messageLogged(const std::string& message, MessageType lvl, const std::list<TraceBack>& traceback,
			                           const std::string& logName) = 0;
		};
		
	}
}
