#pragma once

namespace Engine
{
	namespace Scene
	{
		struct SceneException : std::exception
		{		
			SceneException(const std::string& msg) :
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
