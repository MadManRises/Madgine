#include "../scenelib.h"

#include "lightmanager.h"


#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

namespace Engine
{



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
			return mLights;
		}

		void LightManager::finalize()
		{
			clear();
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


METATABLE_BEGIN(Engine::Scene::LightManager)
METATABLE_END(Engine::Scene::LightManager)



SERIALIZETABLE_BEGIN(Engine::Scene::LightManager)
//mLights -> Serialize::ParentCreator<&LightManager::createLightTuple>
SERIALIZETABLE_END(Engine::Scene::LightManager)

RegisterType(Engine::Scene::LightManager);