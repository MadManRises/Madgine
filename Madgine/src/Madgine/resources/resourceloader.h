#pragma once

#include "resourceloaderbase.h"
#include "resource.h"
#include "../uniquecomponent.h"
#include "resourceloadercollector.h"

namespace Engine
{
	namespace Resources
	{
		
		template <class T, class _Data>
		class ResourceLoader : public UniqueComponent<T, ResourceLoaderCollector>
		{
		public:

			using Data = _Data;
			using Resource = Resource<ResourceLoader<T, Data>>;

			using UniqueComponent<T, ResourceLoaderCollector>::UniqueComponent;


			Resource *get(const std::string &name)
			{
				return &mResources.at(name);
			}

			virtual std::shared_ptr<Data> load(Resource* res) = 0;
			virtual void addResource(const std::experimental::filesystem::path &path) override
			{
				std::string name = path.stem().generic_string();
				auto pib = mResources.try_emplace(name, this, path);
				
				if (!pib.second)
				{
					auto extIndex = [this](const std::string &ext)
					{
						const std::vector<std::string> &extensions = fileExtensions();
						return std::find(extensions.begin(), extensions.end(), ext) - extensions.begin();
					};

					if (extIndex(path.extension().generic_string()) < extIndex(pib.first->second.extension()))
					{
						auto it = mResources.emplace_hint(mResources.erase(pib.first), std::piecewise_construct, std::make_tuple(name), std::make_tuple(this, path));
						resourceAdded(&it->second);
					}
				}else
				{
					resourceAdded(&pib.first->second);
				}
			}

			typename std::map<std::string, Resource>::iterator begin()
			{
				return mResources.begin();
			}

			typename std::map<std::string, Resource>::iterator end()
			{
				return mResources.end();
			}

		private:
			std::map<std::string, Resource> mResources;
		};

	}
}
