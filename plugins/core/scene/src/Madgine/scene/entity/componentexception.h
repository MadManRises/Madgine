#pragma once

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			struct ComponentException : std::exception
			{			
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
