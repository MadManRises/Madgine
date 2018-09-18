#pragma once

#include "traceback.h"
#include "messagetype.h"

namespace Engine
{
	namespace Util
	{

		class INTERFACES_EXPORT UtilMethods
		{
		public:
			static void setup(Log* log = nullptr);
			static void setup(Log* log, bool logToStdOut);

			static void log(const std::string& msg, MessageType lvl);

		private:
			struct __currentLogHolder {
				static thread_local Log* sLog;
				static thread_local bool sLogToStdCout;
			};
		};

#define LOG(s) Engine::Util::UtilMethods::log(s, Engine::Util::LOG_TYPE)
#define LOG_WARNING(s) Engine::Util::UtilMethods::log(s, Engine::Util::WARNING_TYPE)
#define LOG_ERROR(s) Engine::Util::UtilMethods::log(s, Engine::Util::ERROR_TYPE)
#define LOG_EXCEPTION(e) LOG_ERROR(e.what())

#define MADGINE_ABORT() Engine::Util::UtilMethods::abort()
	}
}
