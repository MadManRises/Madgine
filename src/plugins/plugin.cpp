#include "../interfaceslib.h"

#include "plugin.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Engine
{
	namespace Plugins
	{

		Plugin::Plugin(const std::string &path) :
		mModule(nullptr),
		mPath(path)
		{
		}

		bool Plugin::isLoaded()
		{
			return mModule != nullptr;
		}

		bool Plugin::load()
		{
			if (isLoaded())
				return true;

#ifdef _WIN32
			mModule = GetModuleHandle(mPath.c_str());
#else
#error "Unsupported Platform"
#endif

			if (!isLoaded())
				return false;



		}
	}
}