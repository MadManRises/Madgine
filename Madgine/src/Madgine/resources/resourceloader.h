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
			using ResourceType = Resource<ResourceLoader<T, Data>>;

			using UniqueComponent<T, ResourceLoaderCollector>::UniqueComponent;


			ResourceType *get(const std::string &name)
			{
				return &mResources.at(name);
			}

			virtual std::shared_ptr<Data> load(ResourceType* res) = 0;
			virtual void addResource(const std::experimental::filesystem::path &path) override
			{
				std::string name = path.stem().generic_string();
				auto pib = mResources.try_emplace(name, this, path);
				
				if (!pib.second)
				{
					auto extIndex = [this](const std::string &ext)
					{
						const std::vector<std::string> &extensions = this->fileExtensions();
						return std::find(extensions.begin(), extensions.end(), ext) - extensions.begin();
					};

					if (extIndex(path.extension().generic_string()) < extIndex(pib.first->second.extension()))
					{
						auto it = mResources.emplace_hint(mResources.erase(pib.first), std::piecewise_construct, std::make_tuple(name), std::make_tuple(this, path));
						this->resourceAdded(&it->second);
					}
				}else
				{
					this->resourceAdded(&pib.first->second);
				}
			}

			typename std::map<std::string, ResourceType>::iterator begin()
			{
				return mResources.begin();
			}

			typename std::map<std::string, ResourceType>::iterator end()
			{
				return mResources.end();
			}

		private:
			std::map<std::string, ResourceType> mResources;
		};

	}
}
