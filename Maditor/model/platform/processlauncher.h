#pragma once


#ifdef _WIN32
#include <Windows.h>
#undef min
#undef max
#endif

namespace Maditor
{
	namespace Model
	{
		namespace Platform
		{

			class ProcessLauncher : public QObject
			{
				Q_OBJECT

			public:

				static const constexpr size_t PIPE_READ = 0;
				static const constexpr size_t PIPE_WRITE = 0;

#ifdef _WIN32
				using ProcessId = DWORD;
				using ProcessHandle = HANDLE;
				using PipeHandle = HANDLE;
				
#elif __linux__
				using ProcessId = pid_t;
				using ProcessHandle = pid_t;
				using PipeHandle = int;

#endif

			public:
				ProcessLauncher();
				~ProcessLauncher();

				ProcessId pid();
				HANDLE handle();
				bool isLaunched();

			public slots:
				bool kill(int code);
				bool launch(const std::string &cmd, const std::string &cwd = {});
				void sendInput(const std::string &cmd);


			signals:
				void processStarted(ProcessId);
				void processError(int);
				void processKilled();
				void processTerminated();
				void outputReceived(const std::string &msg);
				

			protected:
				void checkProcess();
				void cleanup();
				virtual void timerEvent(QTimerEvent* event) override;

				void closeAllPipes();

			private:

				ProcessId mPID;
				ProcessHandle mHandle;
				PipeHandle mChildOut[2];
				PipeHandle mChildIn[2];

				int mLastExitCode;

			};

		}
	}
}