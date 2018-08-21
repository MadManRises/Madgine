#pragma once

#include "standardlog.h"
#include "../signalslot/slot.h"

namespace Engine
{
	namespace Util
	{
		class MADGINE_BASE_EXPORT ServerLog : public StandardLog
		{
		public:
			ServerLog(const std::string& name);
			~ServerLog();

			// Inherited via Log
			void log(const std::string& msg, MessageType lvl, const std::list<TraceBack>& traceBack = {}) override;

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
