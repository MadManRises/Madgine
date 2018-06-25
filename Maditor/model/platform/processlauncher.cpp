#include "maditormodellib.h"

#include "processlauncher.h"


namespace Maditor
{
	namespace Model
	{
		namespace Platform
		{
			ProcessLauncher::ProcessLauncher() :
				mPID(0),
				mChildInRead(NULL),
				mChildInWrite(NULL),
				mChildOutRead(NULL),
				mChildOutWrite(NULL),
			mLastExitCode(-1)
			{
				//TODO linuc
#ifdef _WIN32

				SECURITY_ATTRIBUTES sa;

				sa.nLength = sizeof(sa);
				sa.bInheritHandle = true;
				sa.lpSecurityDescriptor = NULL;

				if (!CreatePipe(&mChildOutRead, &mChildOutWrite, &sa, 0))
					return;

				if (!SetHandleInformation(mChildOutRead, HANDLE_FLAG_INHERIT, 0))
					return;

				if (!CreatePipe(&mChildInRead, &mChildInWrite, &sa, 0))
					return;

				if (!SetHandleInformation(mChildInWrite, HANDLE_FLAG_INHERIT, 0))
					return;
#endif

				startTimer(10);
			}

			ProcessLauncher::~ProcessLauncher()
			{
				kill(-1);

				if (mChildInRead)
				{
#ifdef _WIN32
					CloseHandle(mChildInRead);
#endif
					mChildInRead = NULL;
				}
				if (mChildInWrite)
				{
#ifdef _WIN32
					CloseHandle(mChildInWrite);
#endif
					mChildInWrite = NULL;
				}
				if (mChildOutRead)
				{
#ifdef _WIN32
					CloseHandle(mChildOutRead);
#endif
					mChildOutRead = NULL;
				}
				if (mChildOutWrite)
				{
#ifdef _WIN32
					CloseHandle(mChildOutWrite);
#endif
					mChildOutWrite = NULL;
				}
			}

			bool ProcessLauncher::launch(const std::string& cmd, const std::string &cwd)
			{

				assert(!isLaunched());

#ifdef _WIN32
				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				// set the size of the structures
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				si.hStdError = mChildOutWrite;
				si.hStdOutput = mChildOutWrite;
				si.hStdInput = mChildInRead;
				si.dwFlags |= STARTF_USESTDHANDLES;

				ZeroMemory(&pi, sizeof(pi));

				// start the program up
				bool result = CreateProcess(NULL, // the path
					const_cast<char*>(cmd.c_str()),
					NULL, // Process handle not inheritable
					NULL, // Thread handle not inheritable
					TRUE, // Set handle inheritance to TRUE
					0, // No creation flags
					NULL, // Use parent's environment block
					cwd.empty() ? NULL : cwd.c_str(),
					&si, // Pointer to STARTUPINFO structure
					&pi // Pointer to PROCESS_INFORMATION structure
				);

				if (!result) {
					emit processError(GetLastError());
					return false;
				}

				mPID = pi.dwProcessId;
				mHandle = pi.hProcess;
				CloseHandle(pi.hThread);
#endif

				emit processStarted(mPID);

				return true;

			}

			void ProcessLauncher::sendInput(const std::string& cmd)
			{
#ifdef _WIN32
				DWORD dwWritten;
				std::string stdCmd = cmd + '\n';
				bool result = WriteFile(mChildInWrite, stdCmd.c_str(), stdCmd.size(), &dwWritten, NULL);
				assert(result && dwWritten == stdCmd.size());
#endif
			}

			void ProcessLauncher::checkProcess()
			{
#ifdef _WIN32
				DWORD exitCode = 0;
				if (GetExitCodeProcess(mHandle, &exitCode) == FALSE)
					throw 0;
				if (exitCode != STILL_ACTIVE)
				{
					mLastExitCode = exitCode;
					cleanup();
				}
#endif
			}

			void ProcessLauncher::cleanup()
			{
				if (mPID)
				{
					mPID = 0;
#ifdef _WIN32
					CloseHandle(mHandle);
#endif
					mHandle = NULL;

					emit processTerminated();
				}
			}

			void ProcessLauncher::timerEvent(QTimerEvent* event)
			{
				QObject::timerEvent(event);
				if (mPID)
				{
					checkProcess();
				}
				if (mPID) {
#ifdef _WIN32
					DWORD dwRead;
					CHAR buffer[256];

					bool result = PeekNamedPipe(mChildOutRead, NULL, 0, NULL, &dwRead, NULL);

					if (result)
					{
						std::string msg;
						while (dwRead > 0)
						{
							DWORD bytesRead;
							result = ReadFile(mChildOutRead, buffer, std::min(sizeof(buffer) - 1, size_t(dwRead)), &bytesRead, NULL);
							assert(result && bytesRead > 0);
							buffer[bytesRead] = '\0';
							msg += buffer;
							dwRead -= bytesRead;
						}

						if (!msg.empty())
						{
							emit outputReceived(msg);
						}
					}
#endif
				}
			}

			HANDLE ProcessLauncher::handle()
			{
				return mHandle;
			}

			pid_t ProcessLauncher::pid()
			{
				return mPID;
			}

			bool ProcessLauncher::isLaunched()
			{
				return mPID != 0;
			}

			bool ProcessLauncher::kill(int code)
			{
				if (mPID)
				{
					checkProcess();
					if (mPID)
					{
#ifdef _WIN32
						TerminateProcess(mHandle, -1);
#endif
						emit processKilled();

						cleanup();

						return true;
					}
				}
				return false;
			}
		}
	}
}