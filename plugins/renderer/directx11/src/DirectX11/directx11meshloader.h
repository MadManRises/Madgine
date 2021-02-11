#pragma once

#include "Madgine/resources/resourceloader.h"

#include "gpumeshloader.h"

#include "directx11meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11MeshLoader : Resources::VirtualResourceLoaderImpl<DirectX11MeshLoader, DirectX11MeshData, GPUMeshLoader> {
        DirectX11MeshLoader();

        void generateImpl(DirectX11MeshData &data, const MeshData &mesh);
        virtual bool generate(GPUMeshData &data, const MeshData &mesh) override;
        virtual bool generate(GPUMeshData &data, MeshData &&mesh) override;

        void updateImpl(DirectX11MeshData &data, const MeshData &mesh);
        virtual void update(GPUMeshData &data, const MeshData &mesh) override;
        virtual void update(GPUMeshData &data, MeshData &&mesh) override;

		virtual void reset(GPUMeshData &data) override;
    };
}
}

RegisterType(Engine::Render::DirectX11MeshLoader);