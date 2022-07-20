#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Madgine/meshloader/gpumeshloader.h"

#include "directx11meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11MeshLoader : Resources::VirtualResourceLoaderImpl<DirectX11MeshLoader, DirectX11MeshData, GPUMeshLoader> {
        DirectX11MeshLoader();

        virtual bool generate(GPUMeshData &data, const MeshData &mesh) override;

        virtual void update(GPUMeshData &data, const MeshData &mesh) override;

        virtual void reset(GPUMeshData &data) override;

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };
}
}

REGISTER_TYPE(Engine::Render::DirectX11MeshLoader)