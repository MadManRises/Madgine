#include "maditormodellib.h"

#include "processlauncher.h"


#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#endif

namespace Maditor
{
	namespace Model
	{
		namespace Platform
		{
			ProcessLauncher::ProcessLauncher() :
				mPID(0),
				mChildIn{0,0},
				mChildOut{0,0},
				mLastExitCode(-1)
			{
				//TODO linux
#ifdef _WIN32

				SECURITY_ATTRIBUTES sa;

				sa.nLength = sizeof(sa);
				sa.bInheritHandle = true;
				sa.lpSecurityDescriptor = NULL;

				if (!CreatePipe(mChildOut + PIPE_READ, mChildOut + PIPE_WRITE, &sa, 0))
				{
					closeAllPipes();
					throw 0;
				}

				if (!SetHandleInformation(mChildOut[PIPE_READ], HANDLE_FLAG_INHERIT, 0)) {
					closeAllPipes();
					throw 0;
				}

				if (!CreatePipe(mChildIn + PIPE_READ, mChildIn + PIPE_WRITE, &sa, 0)) {
					closeAllPipes();
					throw 0;
				}

				if (!SetHandleInformation(mChildIn[PIPE_WRITE], HANDLE_FLAG_INHERIT, 0)) {
					closeAllPipes();
					throw 0;
				}

#elif __linux__

				if (pipe(mChildOut) < 0) {
					closeAllPipes();
					throw 0;
				}

				if (pipe(mChildIn) < 0) {
					closeAllPipes();
					throw 0;
				}

#endif

				startTimer(10);
			}

			ProcessLauncher::~ProcessLauncher()
			{
				kill(-1);


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
				si.hStdError = mChildOut[PIPE_WRITE];
				si.hStdOutput = mChildOut[PIPE_WRITE];
				si.hStdInput = mChildIn[PIPE_READ];
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

#elif __linux__

				mHandle = fork();
				if (mHandle == 0)
				{
					if (dup2(mChildIn[PIPE_READ], STDIN_FILENO) == -1) {
						exit(errno);
					}

					// redirect stdout
					if (dup2(mChildOut[PIPE_WRITE], STDOUT_FILENO) == -1) {
						exit(errno);
					}

					// redirect stderr
					if (dup2(mChildOut[PIPE_WRITE], STDERR_FILENO) == -1) {
						exit(errno);
					}

					closeAllPipes();

					char *argv[] = { NULL };
					char *envp[] = { NULL };

					exit(execve(cmd.c_str(), argv, envp));

				} else if (mHandle > 0)
				{
					mPID = mHandle;
				} else
				{
					emit processError(errno);
					return false;
				}

#endif

				emit processStarted(mPID);

				return true;

			}

			void ProcessLauncher::sendInput(const std::string& cmd)
			{
#ifdef _WIN32
				DWORD dwWritten;
				std::string stdCmd = cmd + '\n';
				bool result = WriteFile(mChildIn[PIPE_WRITE], stdCmd.c_str(), stdCmd.size(), &dwWritten, NULL);
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

					bool result = PeekNamedPipe(mChildOut[PIPE_READ], NULL, 0, NULL, &dwRead, NULL);

					if (result)
					{
						std::string msg;
						while (dwRead > 0)
						{
							DWORD bytesRead;
							result = ReadFile(mChildOut[PIPE_READ], buffer, std::min(sizeof(buffer) - 1, size_t(dwRead)), &bytesRead, NULL);
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

			void ProcessLauncher::closeAllPipes()
			{
				if (mChildIn[PIPE_READ])
				{
#ifdef _WIN32
					CloseHandle(mChildIn[PIPE_READ]);
#endif
					mChildIn[PIPE_READ] = NULL;
				}
				if (mChildIn[PIPE_WRITE])
				{
#ifdef _WIN32
					CloseHandle(mChildIn[PIPE_WRITE]);
#endif
					mChildIn[PIPE_WRITE] = NULL;
				}
				if (mChildOut[PIPE_READ])
				{
#ifdef _WIN32
					CloseHandle(mChildOut[PIPE_READ]);
#endif
					mChildOut[PIPE_READ] = NULL;
				}
				if (mChildOut[PIPE_WRITE])
				{
#ifdef _WIN32
					CloseHandle(mChildOut[PIPE_WRITE]);
#endif
					mChildOut[PIPE_WRITE] = NULL;
				}
			}

			ProcessLauncher::ProcessHandle ProcessLauncher::handle()
			{
				return mHandle;
			}

			ProcessLauncher::ProcessId ProcessLauncher::pid()
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