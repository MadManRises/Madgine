#pragma once

#include "InputWrapper.h"

namespace Maditor {
	namespace Model {


		class ApplicationWrapper {
		public:

			ApplicationWrapper(Watcher::ApplicationWatcher *watcher, ModuleLoader *loader);
			~ApplicationWrapper();

			void load(Project *project, QWindow *target);
			void go();
			void cleanup();
			void shutdown();

			Watcher::InputWrapper *input();

		private:
			void workerLoop();
			void doTask(std::function<void()> task);

			Engine::App::AppSettings *mSettings;
			Engine::App::Application *mApplication;

			std::mutex mMutex;
			std::queue<std::function<void()>> mWorkQueue;

			ModuleLoader *mLoader;

			bool mWork;
			std::thread mWorker;

			Watcher::InputWrapper mInput;

			Watcher::ApplicationWatcher *mWatcher;
		};
	}
}