#include "../baselib.h"

#include "resourcemanager.h"

#include "../core/root.h"

#include "resourcebase.h"

#include "Interfaces/plugins/binaryinfo.h"
#include "Interfaces/plugins/plugin.h"
#include "Interfaces/plugins/pluginmanager.h"

#include "Interfaces/filesystem/api.h"

namespace Engine
{
	namespace Resources
	{
		
		static ResourceManager *sSingleton = nullptr;

		ResourceManager& ResourceManager::getSingleton()
		{
			assert(sSingleton);
			return *sSingleton;
		}

		ResourceManager::ResourceManager() :
		mCollector(*this)
		{
			assert(!sSingleton);
			sSingleton = this;

#ifndef STATIC_BUILD
			Plugins::PluginManager::getSingleton().addListener(this);
#endif
			
			registerResourceLocation(Filesystem::configPath().parentPath() / "data", 50);

#if ANDROID
			registerResourceLocation("assets:", 25);
#endif
		}

		ResourceManager::~ResourceManager()
		{
#ifndef STATIC_BUILD
			Plugins::PluginManager::getSingleton().removeListener(this);
#endif
		}

		void ResourceManager::registerResourceLocation(const Filesystem::Path &path, int priority)
		{

			if (!exists(path))
				return;

			auto[it, b] = mResourcePaths.try_emplace(path, priority);
			if (!b)
				throw 0;

			if (mInitialized)
			{
				updateResources(path, priority);
			}

		}

		void ResourceManager::init()
		{
			mInitialized = true;

			std::map<std::string, ResourceLoaderBase*> loaderByExtension = getLoaderByExtension();

			for (const std::pair<const Filesystem::Path, int> &p : mResourcePaths)
			{
				updateResources(p.first, p.second, loaderByExtension);
			}

		}

#ifndef STATIC_BUILD
		void ResourceManager::onPluginLoad(const Plugins::Plugin * plugin)
		{
			std::map<std::string, ResourceLoaderBase*> loaderByExtension = getLoaderByExtension();

			if (mInitialized)
			{
				for (const std::pair<const Filesystem::Path, int> &p : mResourcePaths)
				{
					updateResources(p.first, p.second, loaderByExtension);
				}
			}

			const Plugins::BinaryInfo *info = static_cast<const Plugins::BinaryInfo*>(plugin->getSymbol("binaryInfo"));	
			Filesystem::Path binPath = info->mBinaryDir;
			bool isLocal = plugin->fullPath().parentPath() == binPath;
			if (isLocal)
				registerResourceLocation(Filesystem::Path(info->mProjectRoot) / "data", 75);
			//else
				//registerResourceLocation(binPath.parent_path() / "data" / plugin->());
		}

		bool ResourceManager::aboutToUnloadPlugin(const Plugins::Plugin * plugin)
		{
			return false;
		}
#endif

		void ResourceManager::updateResources(const Filesystem::Path &path, int priority)
		{
			updateResources(path, priority, getLoaderByExtension());
		}

		void ResourceManager::updateResources(const Filesystem::Path &path, int priority, const std::map<std::string, ResourceLoaderBase*>& loaderByExtension)
		{
			for (Filesystem::Path p : Filesystem::listFilesRecursive(path))
			{
				updateResource(p, priority, loaderByExtension);
			}

		}

		void ResourceManager::updateResource(const Filesystem::Path & path, int priority, const std::map<std::string, ResourceLoaderBase*>& loaderByExtension)
		{
			std::string extension = path.extension();

			auto it = loaderByExtension.find(extension);
			if (it != loaderByExtension.end())
			{
				auto[resource, b] = it->second->addResource(path);

				if (!b)
				{
					int otherPriority = mResourcePaths.at(resource->path());
					if (priority > otherPriority || (priority == otherPriority && it->second->extensionIndex(extension) < it->second->extensionIndex(resource->path().extension())))
					{
						resource->updatePath(path);
					}
				}
			}

		}

		std::map<std::string, ResourceLoaderBase*> ResourceManager::getLoaderByExtension()
		{
			std::map<std::string, ResourceLoaderBase *> loaderByExtension;

			for (const std::unique_ptr<ResourceLoaderBase> &loader : mCollector)
			{
				for (const std::string &ext : loader->fileExtensions())
				{
					loaderByExtension[ext] = loader.get();
				}
			}
			return loaderByExtension;
		}

	}
}
