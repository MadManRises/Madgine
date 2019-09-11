#include "../moduleslib.h"

#include "resourceloaderbase.h"

#include "resourcebase.h"

#include "Modules/reflection/classname.h"


namespace Engine
{
	namespace Resources
	{

		ResourceLoaderBase::ResourceLoaderBase(std::vector<std::string> &&extensions, bool autoLoad) :
			mExtensions(std::forward<std::vector<std::string>>(extensions)),
			mAutoLoad(autoLoad)
		{
		}

		const std::vector<std::string> &ResourceLoaderBase::fileExtensions() const
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