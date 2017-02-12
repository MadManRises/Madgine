#pragma once

#include "serverlog.h"

namespace Engine {
	namespace Server {

		class MADGINE_SERVER_EXPORT BaseServer {
		public:
			BaseServer(const std::string &name);

			int start();

			ServerLog &getLog();

		protected:
			virtual int run() = 0;

		private:
			ServerLog mLog;
			std::string mName;

		};

	}
}