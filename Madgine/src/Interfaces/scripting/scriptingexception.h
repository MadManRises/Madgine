#pragma once

namespace Engine
{
	namespace Scripting
	{
		class ScriptingException : public std::exception
		{
		public:
			ScriptingException(const std::string& msg) :
				mMsg(msg)
			{
			}

			const char* what() const noexcept override
			{
				return mMsg.c_str();
			}

		private:
			std::string mMsg;
		};
	}
}
