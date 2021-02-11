#pragma once

#include "Madgine/resources/resourceloader.h"

#include "render/attributedescriptor.h"
#include "render/vertex.h"

#include "Meta/math/boundingbox.h"

#include "Modules/threading/workgroupstorage.h"

#include "Generic/bytebuffer.h"

#include "meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_MESHLOADER_EXPORT MeshLoader : Resources::ResourceLoader<MeshLoader, MeshData> {

        using Base = ResourceLoader<MeshLoader, MeshData>;

        MeshLoader();

        bool loadImpl(MeshData &data, ResourceType *res);
        void unloadImpl(MeshData &data, ResourceType *res);

        
        
    };

}
}

RegisterType(Engine::Render::MeshLoader);