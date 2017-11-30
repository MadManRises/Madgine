#pragma once

namespace Engine
{
	namespace Scene
	{
		class SceneException : public std::exception
		{
		public:
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
