#include "../interfaceslib.h"

#include <iostream>

#include "log.h"
#include "loglistener.h"
#include "standardlog.h"

namespace Engine
{
	namespace Util
	{

		static StandardLog sStandardLog{ "Madgine" };
		static thread_local Log* sLog = &sStandardLog;
		

		void setLog(Log* log)
		{
			sLog = log ? log : &sStandardLog;
		}

		void log(const std::string& msg, MessageType level)
		{
			sLog->log(msg.c_str(), level);
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
