#pragma once

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			class ParseException : public std::exception
			{
			public:
				ParseException(const std::string& msg) :
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
		} // namespace Parsing
	} // namespace Scripting
}
