#pragma once

#include "Modules/resources/resourceloader.h"

#include "render/attributedescriptor.h"
#include "render/vertex.h"

#include "Modules/math/boundingbox.h"

#include "Modules/threading/workgroupstorage.h"

#include "Modules/generic/bytebuffer.h"

#include "meshdata.h"
#include "..\..\physics\bullet\src\Madgine\physics\collisionshapemanager.h"

namespace Engine {
namespace Render {

    struct MADGINE_MESHLOADER_EXPORT MeshLoader : Resources::ResourceLoader<MeshLoader, MeshData, std::list<Placeholder<0>>> {

        using Base = ResourceLoader<MeshLoader, MeshData, std::list<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct HandleType : Base::HandleType {
            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }
        };

        MeshLoader();

        bool loadImpl(MeshData &data, ResourceType *res);
        void unloadImpl(MeshData &data, ResourceType *res);

        
        
    };

}
}

RegisterType(Engine::Render::MeshLoader);