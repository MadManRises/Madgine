#include "../interfaceslib.h"

#if UNIX

#include <sys/prctl.h>

namespace Engine {
	namespace Threading {

		void setCurrentThreadName(const std::string & name)
		{
			prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
		}


		std::string getCurrentThreadName()
		{
			char buffer[17];
			prctl(PR_GET_NAME, buffer, 0, 0, 0);
			return buffer;
		}

	}
}

#endif