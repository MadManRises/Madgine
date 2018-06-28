#pragma once

#include "resourceloadercollector.h"
#include "resourceloader.h"

namespace Engine
{
	namespace Resources
	{
		class MADGINE_BASE_EXPORT ResourceManager
		{
			
		public:
			ResourceManager(App::Root &root, const std::experimental::filesystem::path &rootDir);

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
				return get<Loader>(name)->loadImpl();
			}

			bool init();

			App::Root &root();

			const std::vector<std::experimental::filesystem::path> &folders() const;

		private:
			App::Root &mRoot;
			ResourceLoaderCollector mCollector;

			std::experimental::filesystem::path mRootDir;
			std::vector<std::experimental::filesystem::path> mFolders;

		};

		
	}
}