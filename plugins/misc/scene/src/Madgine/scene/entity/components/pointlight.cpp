#include "../../../scenelib.h"

#include "pointlight.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/reflect/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

NAMED_UNIQUECOMPONENT(PointLight, Engine::Scene::Entity::PointLight);

METATABLE_BEGIN_BASE(Engine::Scene::Entity::PointLight, Engine::Scene::Entity::EntityComponentBase)
    STORAGE_BEGIN(Engine::Scene::Entity::PointLight, Engine::Scene::Entity::PointLight)
    CONSTRUCTOR()
    STORAGE_END(Engine::Scene::Entity::PointLight)
    MEMBER(mRange)
    MEMBER(mColor)
    MEMBER(mIntensity)
    MEMBER(mCastShadows)
METATABLE_END(Engine::Scene::Entity::PointLight)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::PointLight)
    FIELD(mRange)
    FIELD(mColor)
    FIELD(mIntensity)
    FIELD(mCastShadows)
SERIALIZETABLE_END(Engine::Scene::Entity::PointLight)
