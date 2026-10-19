#include "../../../scenelib.h"

#include "spotlight.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/reflect/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

NAMED_UNIQUECOMPONENT(SpotLight, Engine::Scene::Entity::SpotLight);

METATABLE_BEGIN_BASE(Engine::Scene::Entity::SpotLight, Engine::Scene::Entity::EntityComponentBase)
    STORAGE_BEGIN(Engine::Scene::Entity::SpotLight, Engine::Scene::Entity::SpotLight)
    CONSTRUCTOR()
    STORAGE_END(Engine::Scene::Entity::SpotLight)
    MEMBER(mRange)
    MEMBER(mColor)
    MEMBER(mIntensity)
    MEMBER(mInnerAngle)
    MEMBER(mOuterAngle)
    MEMBER(mCastShadows)
METATABLE_END(Engine::Scene::Entity::SpotLight)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::SpotLight)
    FIELD(mRange)
    FIELD(mColor)
    FIELD(mIntensity)
    FIELD(mInnerAngle)
    FIELD(mOuterAngle)
    FIELD(mCastShadows)
SERIALIZETABLE_END(Engine::Scene::Entity::SpotLight)
