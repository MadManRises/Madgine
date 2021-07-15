#pragma once

#include "Madgine/resources/resourceloader.h"

#include "gpumeshloader.h"

#include "directx12meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12MeshLoader : Resources::VirtualResourceLoaderImpl<DirectX12MeshLoader, DirectX12MeshData, GPUMeshLoader> {
        DirectX12MeshLoader();

        void generateImpl(DirectX12MeshData &data, const MeshData &mesh);
        virtual bool generate(GPUMeshData &data, const MeshData &mesh) override;
        virtual bool generate(GPUMeshData &data, MeshData &&mesh) override;

        void updateImpl(DirectX12MeshData &data, const MeshData &mesh);
        virtual void update(GPUMeshData &data, const MeshData &mesh) override;
        virtual void update(GPUMeshData &data, MeshData &&mesh) override;

		virtual void reset(GPUMeshData &data) override;
    };
}
}

RegisterType(Engine::Render::DirectX12MeshLoader);