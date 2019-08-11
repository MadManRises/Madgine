#include "../../../serverlib.h"

#include "serveranimation.h"

#include "Modules/keyvalue/valuetype.h"

#include "Modules/keyvalue/metatable.h"
#include "Modules/reflection/classname.h"

namespace Engine {

namespace Scene {
    namespace Entity {
        ENTITYCOMPONENTVIRTUALIMPL_IMPL(ServerAnimation);

        ServerAnimation::ServerAnimation(Entity &entity/*, const Scripting::LuaTable &table*/)
            : SerializableUnit(entity/*, table*/)
            , mDefaultAnimation(/*table["default"].asDefault<std::string>(*/""/*)*/)
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

RegisterType(Engine::Scene::Entity::ServerAnimation);