#include "../../../serverlib.h"

#include "serveranimation.h"

#include "Modules/keyvalue/valuetype.h"

#include "Modules/keyvalue/metatable.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

namespace Engine {

namespace Scene {
    namespace Entity {
        ENTITYCOMPONENTVIRTUALIMPL_IMPL(ServerAnimation);

        ServerAnimation::ServerAnimation(Entity &entity, const ObjectPtr &table)
            : SerializableUnit(entity, table)
            , mDefaultAnimation(table["default"].asDefault<std::string>(""))
        {
        }

        ServerAnimation::~ServerAnimation()
        {
        }

        std::string ServerAnimation::getDefaultAnimation() const
        {
            return mDefaultAnimation;
        }

        void ServerAnimation::setDefaultAnimation(const std::string &name)
        {
            mDefaultAnimation = name;
        }

    }
}
}


SERIALIZETABLE_BEGIN(Engine::Scene::Entity::ServerAnimation)
SERIALIZETABLE_END(Engine::Scene::Entity::ServerAnimation)

RegisterType(Engine::Scene::Entity::ServerAnimation);