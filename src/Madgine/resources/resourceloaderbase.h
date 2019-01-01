#pragma once 

#include "Interfaces/reflection/classname.h"

namespace Engine
{
	namespace Resources
	{
	
		class MADGINE_BASE_EXPORT ResourceLoaderBase
		{
		public:
			ResourceLoaderBase(ResourceManager &mgr, std::vector<std::string> &&extensions, bool autoLoad=false);
			virtual ~ResourceLoaderBase() = default;			

			virtual std::pair<ResourceBase *, bool> addResource(const std::experimental::filesystem::path &path) = 0;

			void resourceAdded(ResourceBase* res);
			
			const std::vector<std::string> &fileExtensions() const;

			size_t extensionIndex(const std::string &ext) const;

		private:
			ResourceManager &mMgr;

			std::vector<std::string> mExtensions;

			bool mAutoLoad;

		};

	}
}

RegisterType(Engine::Resources::ResourceLoaderBase);