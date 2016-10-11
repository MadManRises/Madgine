#pragma once

namespace Engine {
	namespace UI {
		class ProcessListener {

		public:
			virtual void onSubProcessStarted(const std::string &name) = 0;
			virtual void onProgressUpdate(float progress) = 0;

		};
	}
}