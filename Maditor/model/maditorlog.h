#pragma once

#include "Madgine/util/log.h"

namespace Maditor {
	namespace Model {


		class MaditorLog : public Engine::Util::Log {
		public:
			MaditorLog(LogTableModel *model);

			// Inherited via Log
			virtual void log(const std::string & msg, Engine::Util::MessageType lvl, const std::list<Engine::Util::TraceBack>& traceBack = {}) override;
			virtual std::string getName() override;

		private:
			LogTableModel *mModel;
		};

	}
}
