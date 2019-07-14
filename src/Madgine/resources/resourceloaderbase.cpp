#include "../baselib.h"

#include "resourceloaderbase.h"

#include "resourcebase.h"

#include "Modules/reflection/classname.h"


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
		size_t ResourceLoaderBase::extensionIndex(const std::string & ext) const
		{			
			return std::find(mExtensions.begin(), mExtensions.end(), ext) - mExtensions.begin();
		}
	}
}

RegisterType(Engine::Resources::ResourceLoaderBase);