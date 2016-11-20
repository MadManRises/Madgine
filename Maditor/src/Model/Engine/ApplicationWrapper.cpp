#include "maditorlib.h"

#include "ApplicationWrapper.h"

#include "Model\Editor.h"


#include "Watcher\ApplicationWatcher.h"

#include "ModuleLoader.h"

#include "Scripting\Parsing\parseexception.h"

#include "Common/Shared.h"

#include <Windows.h>


namespace Maditor {
	namespace Model {
		ApplicationWrapper::ApplicationWrapper(Watcher::ApplicationWatcher *watcher, ModuleLoader *loader) :
			ProcessTalker("Maditor_Launcher", "Maditor"),
			mWatcher(watcher),
			mLoader(loader),
			mAppInfo(SharedMemory::getSingleton().mAppInfo)
		{

			/*std::stringstream ss;
			ss << mWorker.get_id();
			qDebug() << QString::fromStdString(ss.str());*/

			startTimer(500);
		}
		ApplicationWrapper::~ApplicationWrapper()
		{
			shutdown();
		}

		void ApplicationWrapper::load(Project *project, QWindow *target)
		{
			mAppInfo.mMediaDir = (project->path() + "Data/").toStdString().c_str();
			mAppInfo.mProjectDir = project->path().toStdString().c_str();
			mAppInfo.mWindowHandle = (size_t)(target->winId());
			mAppInfo.mWindowWidth = target->width();
			mAppInfo.mWindowHeight = target->height();


			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			// set the size of the structures
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			// start the program up
			CreateProcess(NULL,   // the path
				"Maditor_Madgine_Launcher.exe",
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi           // Pointer to PROCESS_INFORMATION structure
			);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			
			mLoader->setup(project->path() + "bin/", project->moduleList());

		}

		bool ApplicationWrapper::sendMsg(ApplicationCmd cmd)
		{
			ApplicationMsg msg;
			msg.mCmd = cmd;
			return ProcessTalker::sendMsg(msg, "Launcher");
		}


		void ApplicationWrapper::start()
		{
			sendMsg(START_APP);
		}

		void ApplicationWrapper::shutdown()
		{
			sendMsg(SHUTDOWN);
		}

		void ApplicationWrapper::stop()
		{			
			sendMsg(STOP_APP);
		}

		void ApplicationWrapper::resizeWindow()
		{
			sendMsg(RESIZE_WINDOW);
		}

		Watcher::InputWrapper * ApplicationWrapper::input()
		{
			return &mInput;
		}

		void ApplicationWrapper::receiveMessage(const ApplicationMsg & msg)
		{
			switch (msg.mCmd) {
			case APP_CREATED:
				mWatcher->notifyApplicationCreated();
				break;
			case APP_INITIALIZED:
				mWatcher->notifyApplicationInitialized();
				break;
			case APP_STARTED:
				mWatcher->notifyApplicationStarted();
				mInput.setEnabled(true);
				break;
			case APP_STOPPED:
				mInput.setEnabled(false);
				mWatcher->notifyApplicationStopped();
				break;
			case APP_SHUTDOWN:
				mWatcher->notifyApplicationShutdown();
				break;
			case APP_AFTER_SHUTDOWN:
				mWatcher->afterApplicationShutdown();
				mLoader->clear();
				break;
			}
		}

		void ApplicationWrapper::timerEvent(QTimerEvent * te)
		{
			update();
			mWatcher->update();
			mLoader->update();
		}

	}
}