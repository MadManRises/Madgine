#pragma once

#include "resourceloadercollector.h"
#include "Interfaces/uniquecomponent/uniquecomponentcontainer.h"
#include "Interfaces/plugins/pluginlistener.h"
#include "Interfaces/uniquecomponent/uniquecomponent.h"
#include "Interfaces/filesystem/path.h"

namespace Engine
{
	namespace Resources
	{
		class MADGINE_BASE_EXPORT ResourceManager 
#ifndef STATIC_BUILD
			: Plugins::PluginListener
#endif
		{
			
		public:
			static ResourceManager &getSingleton();

			ResourceManager();
			~ResourceManager();

			void registerResourceLocation(const Filesystem::Path &path, int priority);

			template <class Loader>
			typename Loader::ResourceType *get(const std::string &name)
			{
				return mCollector.get<Loader>().get(name);
			}

			template <class Loader>
			Loader &get()
			{
				return mCollector.get<Loader>();
			}

			template <class Loader>
			std::shared_ptr<typename Loader::Data> load(const std::string &name)
			{
				typename Loader::ResourceType *res = get<Loader>(name);
				if (res)
					return res->loadImpl();
				else
					return {};
			}

			bool init();

#ifndef STATIC_BUILD
		protected:
			void onPluginLoad(const Plugins::Plugin *plugin) override;
			bool aboutToUnloadPlugin(const Plugins::Plugin *plugin) override;
#endif

		private:
			void updateResources(const Filesystem::Path &path, int priority);
			void updateResources(const Filesystem::Path &path, int priority, const std::map<std::string, ResourceLoaderBase*> &loaderByExtension);

		private:
			static ResourceManager *sSingleton;

			ResourceLoaderContainer<std::vector> mCollector;

			struct SubDirCompare
			{
				bool operator()(const Filesystem::Path &first, const Filesystem::Path &second) const
				{
					auto[firstEnd, secondEnd] = std::mismatch(first.str().begin(), first.str().end(), second.str().begin(), second.str().end());
					if (firstEnd == first.str().end() || secondEnd == second.str().end())
						return false;
					return first.str() < second.str();
				}
			};

			std::map<Filesystem::Path, int, SubDirCompare> mResourcePaths;

			bool mInitialized = false;

		};

		
	}
}