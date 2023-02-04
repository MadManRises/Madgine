#pragma once

#include "Madgine/resources/resourceloader.h"

#include "skeletondescriptor.h"

namespace Engine {
namespace Render {

    struct MADGINE_SKELETONLOADER_EXPORT SkeletonLoader : Resources::ResourceLoader<SkeletonLoader, SkeletonDescriptor, std::list<Placeholder<0>>> {

        using Base = ResourceLoader<SkeletonLoader, SkeletonDescriptor, std::list<Placeholder<0>>>;

        struct Handle : Base::Handle {
            using Base::Handle::Handle;
            Handle(Base::Handle handle)
                : Base::Handle(std::move(handle))
            {
            }
        };

        SkeletonLoader();

        Threading::Task<bool> loadImpl(SkeletonDescriptor &data, ResourceDataInfo &info);
        Threading::Task<void> unloadImpl(SkeletonDescriptor &data);
    };

}
}

REGISTER_TYPE(Engine::Render::SkeletonLoader)