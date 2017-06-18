#include "interfaceslib.h"
#include "standardlog.h"

#include <iostream>
#include <ctime>
#include <chrono>

namespace Engine {
	namespace Util {

		StandardLog::StandardLog(const std::string & name) :
			mName(name)
		{
		}

		void StandardLog::log(const std::string & msg, Util::MessageType lvl, const std::list<Util::TraceBack>& traceBack)
		{
			std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			struct tm * p = localtime(&time);
			
			const char *type;
			switch (lvl) {
			case Util::LOG_TYPE:
				type = "Log      :";
				break;
			case Util::WARNING_TYPE:
				type = "Warning  :";
				break;
			case Util::ERROR_TYPE:
				type = "Error    :";
				break;
			}

			char s[30];
			strftime(s, 28, "%d/%m/%Y - %H:%M : ", p);
			std::cout << s << msg << std::endl;
		}

		std::string StandardLog::getName()
		{
			return mName;
		}

	}
}


