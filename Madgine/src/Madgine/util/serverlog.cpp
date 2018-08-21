#include "../baselib.h"
#include "serverlog.h"

#include <iostream>

#ifdef WIN32
#include <conio.h>

#include <Windows.h>
#undef min
#undef max
#undef NO_ERROR
#endif

namespace Engine
{
	namespace Util
	{
		ServerLog::ServerLog(const std::string& name) :
			StandardLog(name),
			mCharTypedSlot(this),
			mHandleSlot(this),
			mRunning(false),
			mEcho(false)
		{
		}

		ServerLog::~ServerLog()
		{
			stopConsole();
		}

		void ServerLog::log(const std::string& msg, MessageType lvl, const std::list<TraceBack>& traceBack)
		{
			std::cout << "\r";
			StandardLog::log(msg, lvl, traceBack);
			if (mRunning)
			{
				std::cout << "prompt> " << mCurrentCmd;
				std::cout.flush();
			}
		}

		void ServerLog::runConsole()
		{
#ifdef WIN32
			HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
			std::string cmd;

			DWORD fdwOldMode;
			bool console = GetConsoleMode(input, &fdwOldMode);
			// disable mouse and window input
			DWORD fdwMode = fdwOldMode ^ ENABLE_MOUSE_INPUT ^ ENABLE_WINDOW_INPUT;
			console &= static_cast<bool>(SetConsoleMode(input, fdwMode));

			mEcho = console;

			CHAR buffer[256];
			while (mRunning)
			{
				DWORD dwRead = 0;

				if (console)
				{
					if (WaitForSingleObject(input, 0) == WAIT_OBJECT_0)
					{
						bool result = ReadConsole(input, buffer, sizeof buffer, &dwRead, nullptr);
						assert(result);
					}
				}
				else
				{
					bool result = PeekNamedPipe(input, nullptr, 0, nullptr, &dwRead, nullptr);
					assert(result);

					if (dwRead > 0)
					{
						result = ReadFile(input, buffer, std::min(sizeof buffer, size_t(dwRead)), &dwRead, nullptr);
						assert(result && dwRead > 0);
					}
				}
				if (dwRead > 0)
				{
					for (DWORD i = 0; i < dwRead; ++i)
					{
						char c = buffer[i];

						bool send = false;
						switch (c)
						{
						case '\r':
						case '\n':
							send = true;
							break;
						default:
							cmd += c;
							break;
						}
						mCharTypedSlot.queue(c, cmd);
						if (send && !cmd.empty())
						{
							mHandleSlot.queue(cmd);
							cmd.clear();
						}
					}
				}
				std::this_thread::yield();
			}
			SetConsoleMode(input, fdwOldMode);
#endif
		}

		void ServerLog::handle(const std::string& cmd)
		{
			mCurrentCmd.clear();
			if (!mEvaluator(cmd))
			{
				std::cout << "Unknown Command: " << cmd << std::endl;
			}
			if (mRunning)
			{
				std::cout << "prompt> ";
				std::cout.flush();
			}
		}

		void ServerLog::charTyped(char c, const std::string& cmd)
		{
			mCurrentCmd = cmd;
			if (!mEcho)
			{
				std::cout << c;
				std::cout.flush();
			}
		}


		void ServerLog::startConsole(const std::function<bool(const std::string&)>& evaluator)
		{
			assert(!mConsoleThread.joinable());
			std::cout << "prompt> ";
			std::cout.flush();
			mRunning = true;
			mEvaluator = evaluator;
			mConsoleThread = std::thread(&ServerLog::runConsole, this);
		}

		void ServerLog::stopConsole()
		{
			mRunning = false;
			mConsoleThread.join();
		}
	}
}
