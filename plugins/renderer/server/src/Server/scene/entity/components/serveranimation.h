#pragma once

#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/entitycomponent.h"
#include "Modules/serialize/container/serialized.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        class MADGINE_SERVER_EXPORT ServerAnimation : public EntityComponentVirtualImpl<ServerAnimation, Animation> {
        public:
            ServerAnimation(Entity &entity/*, const Scripting::LuaTable &table*/);
            virtual ~ServerAnimation();

            // Geerbt über SerializableUnit
            virtual std::string getDefaultAnimation() const override;
            virtual void setDefaultAnimation(const std::string &name) override;

        private:
            std::string mDefaultAnimation;
        };
    }
}
}
