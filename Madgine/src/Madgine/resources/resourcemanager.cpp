#include "../baselib.h"

#include "resourcemanager.h"

#include "../core/root.h"

namespace Engine
{
	namespace Resources
	{
		
		ResourceManager::ResourceManager(Core::Root &root, const std::experimental::filesystem::path &rootDir) :
		mCollector(root.pluginMgr(), *this),
		mRoot(root),
		mRootDir(rootDir)
		{
		}

		bool ResourceManager::init()
		{
			std::map<std::string, ResourceLoaderBase *> loaderByExtension;

			for (const std::unique_ptr<ResourceLoaderBase> &loader : mCollector)
			{
				for (const std::string &ext : loader->fileExtensions())
				{
					loaderByExtension[ext] = loader.get();
				}
			}

			for (auto &p : std::experimental::filesystem::recursive_directory_iterator(mRootDir))
			{
				if (is_regular_file(p))
				{
					std::experimental::filesystem::path path = p.path();
					std::string extension = path.extension().generic_string();

					auto it = loaderByExtension.find(extension);
					if (it != loaderByExtension.end())
					{
						it->second->addResource(path);
					}

				}else if (is_directory(p))
				{
					mFolders.push_back(p);
				}
				
			}

			return true;
		}

		Core::Root& ResourceManager::root()
		{
			return mRoot;
		}

		const std::vector<std::experimental::filesystem::path>& ResourceManager::folders() const
		{
			return mFolders;
		}
	}
}
