#pragma once

#include "log.h"

namespace Engine
{
	namespace Util
	{
		class MODULES_EXPORT StandardLog : public Log
		{
		public:
			StandardLog(const std::string& name);

			static void sLog(const std::string &msg, MessageType lvl, const std::string &name = "Default");

			// Inherited via Log
			void log(const std::string& msg, MessageType lvl) override;
			std::string getName() override;

		private:
			std::string mName;
		};
	}
}
