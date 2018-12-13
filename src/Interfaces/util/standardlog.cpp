#include "../interfaceslib.h"
#include "standardlog.h"

#include <iostream>

namespace Engine
{
	namespace Util
	{
		StandardLog::StandardLog(const std::string& name) :
			mName(name)
		{
		}

		void StandardLog::log(const std::string& msg, MessageType lvl)
		{
			time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			struct tm p;
			localtime_s(&p, &time);

			const char* type;
			switch (lvl)
			{
			case LOG_TYPE:
				type = "Log      :";
				break;
			case WARNING_TYPE:
				type = "Warning  :";
				break;
			case ERROR_TYPE:
				type = "Error    :";
				break;
			}

			char s[30];
			strftime(s, 28, "%d/%m/%Y - %H:%M : ", &p);
			std::cout << s << msg << std::endl;
			Log::log(msg, lvl);
		}

		std::string StandardLog::getName()
		{
			return mName;
		}
	}
}