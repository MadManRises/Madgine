#include "maditorlib.h"

#include "ApplicationWrapper.h"

#include "Model\Editor.h"



#include "ModuleLoader.h"

#include "Scripting\Parsing\parseexception.h"

#include "Shared.h"

#include <Windows.h>

#include <chrono>
#include <thread>


namespace Maditor {
	namespace Model {
		ApplicationWrapper::ApplicationWrapper() :
			mAppInfo(SharedMemory::getSingleton().mAppInfo),
			mPID(0)
		{
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

			mPID = pi.dwProcessId;
			mHandle = pi.hProcess;
			CloseHandle(pi.hThread);
			
			for (const auto& f : mProcessListener) {
				f(mPID);
			}

			/*mWatcher->client()->connectToHost();

			mLoader->setup(project->path() + "debug/bin/", project->moduleList());			*/

			
		}



		void ApplicationWrapper::start()
		{
			//sendMsg(START_APP);
		}

		void ApplicationWrapper::shutdown()
		{
			//sendMsg(SHUTDOWN);
		}

		void ApplicationWrapper::stop()
		{			
			//sendMsg(STOP_APP);
		}

		void ApplicationWrapper::resizeWindow()
		{
			//sendMsg(RESIZE_WINDOW);
		}

		Watcher::InputWrapper * ApplicationWrapper::input()
		{
			return &mInput;
		}

		void ApplicationWrapper::timerEvent(QTimerEvent * te)
		{
			if (mPID) {
				DWORD exitCode = 0;
				if (GetExitCodeProcess(mHandle, &exitCode) == FALSE)
					throw 0;
				if (exitCode != STILL_ACTIVE) {
					cleanup();
					return;
				}
			}
		}

		DWORD ApplicationWrapper::pid()
		{
			return mPID;
		}

		void ApplicationWrapper::addProcessListener(std::function<void(DWORD)> f)
		{
			mProcessListener.push_back(f);
		}

		void ApplicationWrapper::cleanup()
		{

			//mWatcher->notifyApplicationShutdown();
			if (mPID) {
				mPID = 0;
				CloseHandle(mHandle);
			}
		}

	}
}