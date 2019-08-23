#include "../serverlib.h"

#include "serverlightmanager.h"

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

namespace Engine
{



	namespace Scene
	{
		ServerLightManager::ServerLightManager(SceneManager &sceneMgr) :
			VirtualSceneComponentImpl<Engine::Scene::ServerLightManager, Engine::Scene::LightManager>(sceneMgr)
		{
		}

		ServerLightManager::~ServerLightManager()
		{
		}

		std::unique_ptr<Light> ServerLightManager::createLightImpl()
		{
			return std::make_unique<Light>();
		}

	}
}


METATABLE_BEGIN(Engine::Scene::ServerLightManager)
METATABLE_END(Engine::Scene::ServerLightManager)


SERIALIZETABLE_BEGIN(Engine::Scene::ServerLightManager)
SERIALIZETABLE_END(Engine::Scene::ServerLightManager)

RegisterType(Engine::Scene::ServerLightManager);