#pragma once

#include "Modules/resources/resourceloader.h"

#include "animationdescriptor.h"

namespace Engine {
namespace Render {

    struct MADGINE_ANIMATIONLOADER_EXPORT AnimationLoader : Resources::ResourceLoader<AnimationLoader, AnimationList, std::list<Placeholder<0>>> {

        using Base = ResourceLoader<AnimationLoader, AnimationList, std::list<Placeholder<0>>>;

        struct HandleType : Base::HandleType {
            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

        };

        AnimationLoader();

        bool loadImpl(AnimationList &data, ResourceType *res);
        void unloadImpl(AnimationList &data, ResourceType *res);

    };

}
}