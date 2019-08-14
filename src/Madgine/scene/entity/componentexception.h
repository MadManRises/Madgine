#pragma once

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class ComponentException : public std::exception
			{
			public:
				ComponentException(const std::string& msg) :
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
}
