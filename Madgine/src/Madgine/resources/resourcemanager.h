#pragma once

#include "resourceloadercollector.h"
#include "../uniquecomponentcollectorinstance.h"

namespace Engine
{
	namespace Resources
	{
		class MADGINE_BASE_EXPORT ResourceManager
		{
			
		public:
			static ResourceManager &getSingleton();

			ResourceManager(Plugins::PluginManager &pluginMgr, const std::experimental::filesystem::path &rootDir);

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

			const std::vector<std::experimental::filesystem::path> &folders() const;

		private:
			static ResourceManager *sSingleton;

			ResourceLoaderCollectorInstance mCollector;

			std::experimental::filesystem::path mRootDir;
			std::vector<std::experimental::filesystem::path> mFolders;

		};

		
	}
}