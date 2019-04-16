#pragma once

#include "Interfaces/util/standardlog.h"
#include "Interfaces/signalslot/slot.h"

namespace Engine
{
	namespace Server
	{
		class MADGINE_BASE_EXPORT ServerLog : public Util::StandardLog
		{
		public:
			ServerLog(const std::string& name);
			~ServerLog();

			// Inherited via Log
			void log(const std::string& msg, Util::MessageType lvl) override;

			bool startConsole();
			void stopConsole();

			std::vector<std::string> update();

		private:

		private:

			bool mEcho;
			std::string mCurrentCmd;
		};
	}
}
