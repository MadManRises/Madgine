#pragma once

#include "InputWrapper.h"

#include "Common\ProcessTalker.h"

#include "Common\ApplicationInfo.h"


namespace Maditor {
	namespace Model {


		class MADITOR_EXPORT ApplicationWrapper : public QObject, public ProcessTalker<ApplicationMsg> {
			Q_OBJECT

		public:

			ApplicationWrapper(Watcher::ApplicationWatcher *watcher, ModuleLoader *loader);
			~ApplicationWrapper();

			void load(Project *project, QWindow *target);
			void start();
			void shutdown();
			void stop();

			void resizeWindow();

			Watcher::InputWrapper *input();

			// Geerbt über ProcessTalker
			virtual void receiveMessage(const ApplicationMsg & msg) override;

			bool sendMsg(ApplicationCmd cmd);

			
			virtual void timerEvent(QTimerEvent *te) override;

			DWORD pid();

			void addProcessListener(std::function<void(DWORD)> f);

		private:
			void cleanup();

		private:

			ModuleLoader *mLoader;

			DWORD mPID;
			HANDLE mHandle;

			Watcher::InputWrapper mInput;

			Watcher::ApplicationWatcher *mWatcher;

			ApplicationInfo &mAppInfo;

			std::list<std::function<void(DWORD)>> mProcessListener;

		};
	}
}