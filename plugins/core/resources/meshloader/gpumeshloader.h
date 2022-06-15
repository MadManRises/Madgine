#pragma once

#include "Madgine/resources/resourceloader.h"

#include "gpumeshdata.h"

#include "Modules/threading/workgroupstorage.h"

#include "meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_MESHLOADER_EXPORT GPUMeshLoader : Resources::VirtualResourceLoaderBase<GPUMeshLoader, GPUMeshData, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {

        using Base = VirtualResourceLoaderBase<GPUMeshLoader, GPUMeshData, std::list<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct PtrType : Base::PtrType {
            using Base::PtrType::PtrType;
            PtrType(Base::PtrType ptr)
                : Base::PtrType(std::move(ptr))
            {
            }

            Threading::TaskFuture<bool> create(MeshData mesh)
            {
                return Base::PtrType::create([mesh { std::move(mesh) }](Render::GPUMeshLoader *loader, Render::GPUMeshData &data) mutable { return loader->generate(data, std::move(mesh)); });
            }

            void update(const MeshData &mesh, GPUMeshLoader *loader = nullptr)
            {
                if (!loader)
                    loader = &GPUMeshLoader::getSingleton();
                loader->update(**this, mesh);
            }
        };

        GPUMeshLoader();

        Threading::Task<bool> loadImpl(GPUMeshData &data, ResourceDataInfo &info);
        void unloadImpl(GPUMeshData &data);

        virtual bool generate(GPUMeshData &data, const MeshData &mesh) = 0;

        virtual void update(GPUMeshData &data, const MeshData &mesh) = 0;

        virtual void reset(GPUMeshData &data) = 0;
    };

}
}

REGISTER_TYPE(Engine::Render::GPUMeshLoader)