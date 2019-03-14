#include "../interfaceslib.h"
#include "standardlog.h"

#include <iostream>

#include <time.h>

#if ANDROID
#include <android/log.h>
#endif

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
#if ANDROID
			int prio;
			switch (lvl)
			{
			case LOG_TYPE:
				prio = ANDROID_LOG_INFO;
				break;
			case WARNING_TYPE:
				prio = ANDROID_LOG_WARN;
				break;
			case ERROR_TYPE:
				prio = ANDROID_LOG_ERROR;
				break;
			default:
				throw 0;
			}
			__android_log_print(prio, mName.c_str(), "%s", msg.c_str());
#else

			time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			
			struct tm *t; 
#if WINDOWS
			struct tm _t;
			t = &_t;
			localtime_s(t, &time);
#else
			t = localtime(&time);
#endif

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
			strftime(s, 28, "%d/%m/%Y - %H:%M : ", t);
			std::cout << s << msg << std::endl;
#endif
			Log::log(msg, lvl);
		}

		std::string StandardLog::getName()
		{
			return mName;
		}
	}
}
