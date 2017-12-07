#include "baselib.h"

#include "profile.h"
#include "profiler.h"


namespace Engine
{
	namespace Util
	{

		ProfileWrapper::ProfileWrapper(const std::string& name)
		{
			Profiler* p = Profiler::getSingletonPtr();
			if (p)
				p->startProfiling(name);
		}

		ProfileWrapper::~ProfileWrapper()
		{
			Profiler* p = Profiler::getSingletonPtr();
			if (p)
				p->stopProfiling();
		}

	}
}
