#pragma once

#include "util/standardlog.h"
#include "signalslot/slot.h"

namespace Engine
{
	namespace Util
	{
		class MADGINE_SERVER_EXPORT ServerLog : public StandardLog
		{
		public:
			ServerLog(const std::string& name);
			~ServerLog();

			// Inherited via Log
			void log(const std::string& msg, MessageType lvl, const std::list<TraceBack>& traceBack = {}) override;

			void startConsole(bool& flag, const std::function<bool(const std::string&)>& evaluator);

		private:
			void runConsole();

			void handle(const std::string& cmd);

			void charTyped(char, const std::string&);

		private:
			SignalSlot::Slot<decltype(&ServerLog::charTyped), &ServerLog::charTyped> mCharTypedSlot;
			SignalSlot::Slot<decltype(&ServerLog::handle), &ServerLog::handle> mHandleSlot;

			bool* mFlag;

			bool mEcho;
			std::string mCurrentCmd;
			std::thread mConsoleThread;
			std::function<bool(const std::string&)> mEvaluator;
		};
	}
}
