#include "../baselib.h"

#include "resourceloaderbase.h"

#include "resourcebase.h"

namespace Engine
{
	namespace Resources
	{

		ResourceLoaderBase::ResourceLoaderBase(ResourceManager& mgr, std::vector<std::string> &&extensions, bool autoLoad) :
			mMgr(mgr),
			mExtensions(std::forward<std::vector<std::string>>(extensions)),
			mAutoLoad(autoLoad)
		{
		}

		void ResourceLoaderBase::resourceAdded(ResourceBase* res)
		{
			if (mAutoLoad)
			{
				res->setPersistent(true);
				res->load();
			}
		}

		const std::vector<std::string>& ResourceLoaderBase::fileExtensions() const
		{
			return mExtensions;
		}
	}
}