#pragma once

#include "resource.h"
#include "Interfaces/uniquecomponent/uniquecomponent.h"
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
				auto it = mResources.find(name);
				if (it != mResources.end())
					return &it->second;
				else
					return nullptr;
			}

			virtual std::shared_ptr<Data> load(ResourceType* res) = 0;
			virtual std::pair<ResourceBase *,bool> addResource(const Filesystem::Path &path) override
			{
				std::string name = path.stem();
				auto pib = mResources.try_emplace(name, this, path);
				
				if (pib.second)				
					this->resourceAdded(&pib.first->second);
				
				return std::make_pair(&pib.first->second, pib.second);
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
