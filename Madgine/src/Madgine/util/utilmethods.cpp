#include "../interfaceslib.h"

#include <iostream>

#include "log.h"
#include "loglistener.h"

namespace Engine
{
	namespace Util
	{

		thread_local Log* UtilMethods::__currentLogHolder::sLog = nullptr;
		thread_local bool UtilMethods::__currentLogHolder::sLogToStdCout = true;

		void UtilMethods::setup(Log* log)
		{
			__currentLogHolder::sLog = log;
		}


		void UtilMethods::setup(Log* log, bool logToStdCout)
		{
			__currentLogHolder::sLog = log;
			__currentLogHolder::sLogToStdCout = logToStdCout;
		}

		void UtilMethods::log(const std::string& msg, MessageType level)
		{
			if (__currentLogHolder::sLog)
				__currentLogHolder::sLog->log(msg.c_str(), level);
			else if (__currentLogHolder::sLogToStdCout)
				std::cout << msg << std::endl;
		}

	}
}
