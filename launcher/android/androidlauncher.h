#pragma once

struct ANativeActivity;
struct ANativeWindow;
struct AInputQueue;

namespace Engine {
	namespace Android {

		struct AndroidLauncher {

			AndroidLauncher(ANativeActivity *activity);

		protected:
			void go();

			void onDestroy();
			void onNativeWindowCreated(ANativeWindow *window);
			void onInputQueueCreated(AInputQueue *queue);
			void onInputQueueDestroyed(AInputQueue *queue);

		private:
			ANativeActivity *mActivity;
			std::thread mThread;
		};

	}
}