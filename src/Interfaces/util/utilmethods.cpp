#include "../interfaceslib.h"

#include <iostream>

#include "log.h"
#include "loglistener.h"
#include "standardlog.h"

namespace Engine
{
	namespace Util
	{

		static Log* sLog = nullptr;
		

		void setLog(Log* log)
		{
			sLog = log;
		}

		void log(const std::string& msg, MessageType level)
		{
			if (sLog)
				sLog->log(msg.c_str(), level);
			else
				StandardLog::sLog(msg, level);
		}

		LogDummy::LogDummy(MessageType lvl) :
			mLvl(lvl)
		{
		}

		LogDummy::~LogDummy()
		{
			log(mStream.str(), mLvl);
		}


	}
}
