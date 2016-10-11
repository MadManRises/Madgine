#ifndef CEGUIEXCEPTION_H
#define CEGUIEXCEPTION_H

#include <exception>
#include <string>

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiException : public std::exception {
			public:
				CeguiException(const std::string &msg) :
					m_msg(msg)
				{
				}


				virtual const char *what() const throw()
				{
					return m_msg.c_str();
				}

			private:
				std::string m_msg;
			};
		}
	}
}

#endif // CEGUIEXCEPTION_H
