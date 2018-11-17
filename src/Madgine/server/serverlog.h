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

			void startConsole(const std::function<bool(const std::string&)>& evaluator);
			void stopConsole();

		private:
			void runConsole();

			void handle(const std::string& cmd);

			void charTyped(char, const std::string&);

		private:
			SignalSlot::Slot<&ServerLog::charTyped> mCharTypedSlot;
			SignalSlot::Slot<&ServerLog::handle> mHandleSlot;

			bool mRunning;

			bool mEcho;
			std::string mCurrentCmd;
			std::thread mConsoleThread;
			std::function<bool(const std::string&)> mEvaluator;
		};
	}
}
