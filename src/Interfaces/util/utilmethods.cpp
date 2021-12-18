#include "../interfaceslib.h"

#include "log.h"
#include "standardlog.h"

namespace Engine
{
	namespace Util
	{

		Log* sLog = nullptr;
		

		void setLog(Log* log)
		{
			sLog = log;
		}

		void log(std::string_view msg, MessageType level)
		{
			if (sLog)
				sLog->log(msg, level);
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
