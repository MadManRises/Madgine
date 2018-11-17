#pragma once

#include "log.h"

namespace Engine
{
	namespace Util
	{
		class INTERFACES_EXPORT StandardLog : public Log
		{
		public:
			StandardLog(const std::string& name);

			// Inherited via Log
			void log(const std::string& msg, MessageType lvl) override;
			std::string getName() override;

		private:
			std::string mName;
		};
	}
}
