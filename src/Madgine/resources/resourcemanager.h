#pragma once

#include "resourceloadercollector.h"
#include "../uniquecomponent/uniquecomponentcontainer.h"
#include "Interfaces/plugins/pluginlistener.h"
#include "../uniquecomponent/uniquecomponent.h"

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

			void registerResourceLocation(const std::experimental::filesystem::path &path, int priority);

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
			void updateResources(const std::experimental::filesystem::path &path, int priority);
			void updateResources(const std::experimental::filesystem::path &path, int priority, const std::map<std::string, ResourceLoaderBase*> &loaderByExtension);

		private:
			static ResourceManager *sSingleton;

			ResourceLoaderContainer mCollector;

			struct SubDirCompare
			{
				bool operator()(const std::experimental::filesystem::path &first, const std::experimental::filesystem::path &second) const
				{
					auto[firstEnd, secondEnd] = std::mismatch(first.begin(), first.end(), second.begin(), second.end());
					if (firstEnd == first.end() || secondEnd == second.end())
						return false;
					return first < second;
				}
			};

			std::map<std::experimental::filesystem::path, int, SubDirCompare> mResourcePaths;

			bool mInitialized = false;

		};

		
	}
}