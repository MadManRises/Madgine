#include "../baselib.h"

#include "lightmanager.h"

namespace Engine
{
	//VIRTUALSCENECOMPONENTBASE(Scene::LightManager);

	API_IMPL(Scene::LightManager);

	namespace Scene
	{
		LightManager::~LightManager()
		{
		}

		Light* Scene::LightManager::createLight()
		{
			auto pib = mLights.emplace_back(createLightImpl());
			if (!pib.second)
				return nullptr;
			else
				return pib.first->get();
		}

		const std::list<std::unique_ptr<Light>> &LightManager::lights() const
		{
			return mLights.data();
		}

		void LightManager::clear()
		{
			mLights.clear();
		}

		std::tuple<std::unique_ptr<Light>> LightManager::createLightTuple()
		{
			return std::make_tuple(createLightImpl());
		}
	}	
}
