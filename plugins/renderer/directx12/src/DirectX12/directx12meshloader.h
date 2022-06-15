#pragma once

#include "Madgine/resources/resourceloader.h"

#include "gpumeshloader.h"

#include "directx12meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12MeshLoader : Resources::VirtualResourceLoaderImpl<DirectX12MeshLoader, DirectX12MeshData, GPUMeshLoader> {
        DirectX12MeshLoader();

        virtual bool generate(GPUMeshData &data, const MeshData &mesh) override;

        virtual void update(GPUMeshData &data, const MeshData &mesh) override;

        virtual void reset(GPUMeshData &data) override;
    };
}
}

REGISTER_TYPE(Engine::Render::DirectX12MeshLoader)