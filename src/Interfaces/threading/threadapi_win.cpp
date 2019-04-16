#include "../interfaceslib.h"

#if WINDOWS

#define NOMINMAX
#include <Windows.h>

#include "threadapi.h"

namespace Engine {
	namespace Threading {

		void setCurrentThreadName(const std::string & name)
		{
			std::wstring wstring;
			wstring.reserve(name.size());
			std::copy(name.begin(), name.end(), std::back_inserter(wstring));
			SetThreadDescription(
				GetCurrentThread(),
				wstring.c_str()
			);			
		}

	}
}

#endif