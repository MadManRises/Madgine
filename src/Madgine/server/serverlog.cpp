#include "../baselib.h"
#include "serverlog.h"

#include <iostream>

#if WINDOWS
#include <conio.h>

#define NOMINMAX
#include <Windows.h>
#undef NO_ERROR
#endif

namespace Engine
{
	namespace Server
	{
		ServerLog::ServerLog(const std::string& name) :
			StandardLog(name),
			mEcho(false)
		{
		}

		ServerLog::~ServerLog()
		{
			stopConsole();
		}

		void ServerLog::log(const std::string& msg, Util::MessageType lvl)
		{
			std::cout << "\r";
			StandardLog::log(msg, lvl);
			std::cout << "prompt> " << mCurrentCmd;
			std::cout.flush();
		}



		static DWORD fdwOldMode;		

		bool ServerLog::startConsole()
		{
			std::cout << "prompt> ";
			std::cout.flush();
			HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
			std::string cmd;

			
			bool console = GetConsoleMode(input, &fdwOldMode);
			// disable mouse and window input
			DWORD fdwMode = fdwOldMode & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
			console &= static_cast<bool>(SetConsoleMode(input, fdwMode));

			mEcho = console;

			return true;
		}

		void ServerLog::stopConsole()
		{
			HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
			SetConsoleMode(input, fdwOldMode);
		}

		std::vector<std::string> ServerLog::update()
		{
			std::vector<std::string> cmds;

#if WINDOWS	

			CHAR buffer[256];

			DWORD dwRead = 0;

			HANDLE input = GetStdHandle(STD_INPUT_HANDLE);

			if (mEcho)
			{
				if (WaitForSingleObject(input, 0) == WAIT_OBJECT_0)
				{
					DWORD count;
					bool result = GetNumberOfConsoleInputEvents(input, &count);
					assert(result);
					if (count > 0)
					{
						INPUT_RECORD record[100];
						result = ReadConsoleInput(input, record, 100, &count);
						assert(result);

						for (DWORD i = 0; i < count; ++i) {
							if (record[i].EventType == KEY_EVENT && record[i].Event.KeyEvent.bKeyDown)
							{
								char c = record[i].Event.KeyEvent.uChar.AsciiChar;
								bool send = false;
								switch (c)
								{
								case '\r':
								case '\n':
									send = true;
									break;
								default:
									mCurrentCmd += c;
									std::cout << c;
									break;
								}

								if (send && !mCurrentCmd.empty())
								{
									cmds.emplace_back(std::move(mCurrentCmd));
									mCurrentCmd.clear();
									std::cout << std::endl;
									std::cout << "prompt> ";
									std::cout.flush();
								}
							}
						}
					}
					
				}
			}
			else
			{
				bool result = PeekNamedPipe(input, nullptr, 0, nullptr, &dwRead, nullptr);
				assert(result);

				if (dwRead > 0)
				{
					result = ReadFile(input, buffer, std::min(static_cast<DWORD>(sizeof buffer), dwRead), &dwRead, nullptr);
					assert(result && dwRead > 0);
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
							mCurrentCmd += c;
							if (mEcho)
								std::cout << c;
							break;
						}

						if (send && !mCurrentCmd.empty())
						{
							cmds.emplace_back(std::move(mCurrentCmd));
							mCurrentCmd.clear();
							std::cout << "prompt> ";
							std::cout.flush();
						}
					}
				}
			}		

#endif
			return cmds;
			
		}
	}
}
