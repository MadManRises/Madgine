#pragma once


#ifdef _WIN32
#include <Windows.h>
#define pid_t DWORD
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
				ProcessLauncher();
				~ProcessLauncher();

				pid_t pid();
				HANDLE handle();
				bool isLaunched();

			public slots:
				bool kill(int code);
				bool launch(const std::string &cmd, const std::string &cwd = {});
				void sendInput(const std::string &cmd);


			signals:
				void processStarted(pid_t);
				void processError(int);
				void processKilled();
				void processTerminated();
				void outputReceived(const std::string &msg);
				

			protected:
				void checkProcess();
				void cleanup();
				virtual void timerEvent(QTimerEvent* event) override;

			private:
				pid_t mPID;
				HANDLE mHandle;
				HANDLE mChildOutRead, mChildOutWrite, mChildInRead, mChildInWrite;

				int mLastExitCode;

			};

		}
	}
}