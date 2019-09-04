#pragma once 

#include "Modules/keyvalue/scopebase.h"

namespace Engine
{
	namespace Resources
	{
	
		struct MADGINE_BASE_EXPORT ResourceLoaderBase : ScopeBase
		{
			ResourceLoaderBase(std::vector<std::string> &&extensions, bool autoLoad=false);
			virtual ~ResourceLoaderBase() = default;			

			virtual std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path) = 0;

			void resourceAdded(ResourceBase* res);
			
			const std::vector<std::string> &fileExtensions() const;

			size_t extensionIndex(const std::string &ext) const;

		private:
			std::vector<std::string> mExtensions;

			bool mAutoLoad;

		};

	}
}