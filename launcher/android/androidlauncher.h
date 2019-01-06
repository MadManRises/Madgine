#pragma once

struct ANativeActivity;

namespace Engine {
	namespace Android {

		struct AndroidLauncher {

			AndroidLauncher(ANativeActivity *activity);

		protected:
			void go();

			void onDestroy();

		private:
			ANativeActivity *mActivity;
			std::thread mThread;
		};

	}
}