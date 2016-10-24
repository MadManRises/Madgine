#pragma once

#include "Model\log.h"

namespace Maditor {
	namespace Model {
		
		class ProjectLog : public Log {
		public:
			virtual std::string getName() override;
			void logMessage(const std::string &msg);
		};

	}
}