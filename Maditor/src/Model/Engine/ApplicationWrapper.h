#pragma once

#include "InputWrapper.h"

#include "ModuleLoader.h"

class ApplicationInfo;


namespace Maditor {
	namespace Model {


		class MADITOR_EXPORT ApplicationWrapper : public QObject {
			Q_OBJECT

		public:

			ApplicationWrapper();
			~ApplicationWrapper();

			void load(Project *project, QWindow *target);
			void start();
			void shutdown();
			void stop();

			void resizeWindow();

			Watcher::InputWrapper *input();

			virtual void timerEvent(QTimerEvent *te) override;

			DWORD pid();

			void addProcessListener(std::function<void(DWORD)> f);

		private:
			void cleanup();

		private:

			ModuleLoader mLoader;

			DWORD mPID;
			HANDLE mHandle;

			Watcher::InputWrapper mInput;

			ApplicationInfo &mAppInfo;

			std::list<std::function<void(DWORD)>> mProcessListener;
		};
	}
}