#include "../meshloaderlib.h"

#include "gpumeshloader.h"

#include "meshloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/threading/taskqueue.h"

METATABLE_BEGIN(Engine::Render::GPUMeshLoader)
METATABLE_END(Engine::Render::GPUMeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::GPUMeshLoader::Resource, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Render::GPUMeshLoader::Resource)

METATABLE_BEGIN(Engine::Render::GPUMeshData)
MEMBER(mMaterials)
METATABLE_END(Engine::Render::GPUMeshData)

METATABLE_BEGIN(Engine::Render::GPUMeshData::Material)
READONLY_PROPERTY(Name, mName)
/* MEMBER(mDiffuseTexture)
MEMBER(mEmissiveTexture)*/
MEMBER(mDiffuseColor)
METATABLE_END(Engine::Render::GPUMeshData::Material)

VIRTUALUNIQUECOMPONENTBASE(Engine::Render::GPUMeshLoader)

namespace Engine {
namespace Render {

    GPUMeshLoader::GPUMeshLoader()
        : VirtualResourceLoaderBase({ ".fbx", ".dae", ".stl" })
    {
    }

    Threading::Task<bool> GPUMeshLoader::loadImpl(GPUMeshData &mesh, ResourceDataInfo &info)
    {
        MeshLoader::Handle handle;
        if (!co_await handle.load(info.resource()->name()))
            co_return false;
        co_return co_await generate(mesh, *handle);
    }

    void GPUMeshLoader::unloadImpl(GPUMeshData &data)
    {
        reset(data);
    }

    Threading::Task<bool> GPUMeshLoader::generate(GPUMeshData &data, const MeshData &mesh)
    {
        data.mFormat = mesh.mFormat;
        data.mVertexSize = mesh.mVertexSize;
        data.mAABB = mesh.mAABB;

        data.mGroupSize = mesh.mGroupSize;

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            data.mElementCount = mesh.mIndices.size();
        }

        data.mMaterials.clear();
        for (const MeshData::Material &mat : mesh.mMaterials) {
            GPUMeshData::Material &gpuMat = data.mMaterials.emplace_back();
            gpuMat.mName = mat.mName;
            gpuMat.mDiffuseColor = mat.mDiffuseColor;

            std::vector<Threading::TaskFuture<bool>> futures;

            futures.push_back(gpuMat.mDiffuseTexture.loadFromImage(mat.mDiffuseName.empty() ? "blank_black" : mat.mDiffuseName, TextureType_2D, FORMAT_RGBA8_SRGB));

            futures.push_back(gpuMat.mEmissiveTexture.loadFromImage(mat.mEmissiveName.empty() ? "blank_black" : mat.mEmissiveName, TextureType_2D, FORMAT_RGBA8_SRGB));

            for (Threading::TaskFuture<bool> &fut : futures) {
                bool result = co_await fut;
                if (!result)
                    co_return false;
            }

            gpuMat.mResourceBlock = createResourceBlock({ &*gpuMat.mDiffuseTexture, &*gpuMat.mEmissiveTexture });
        }

        co_return true;
    }

}
}