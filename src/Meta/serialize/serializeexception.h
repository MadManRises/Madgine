#pragma once


namespace Engine
{
	namespace Serialize
	{
		struct SerializeException : std::exception
		{		
			SerializeException(const std::string& msg) :
				m_msg(msg)
			{
			}


			const char* what() const throw() override
			{
				return m_msg.c_str();
			}

		private:
			std::string m_msg;
		};
	}
}
