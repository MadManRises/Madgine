#include "meshloaderlib.h"

#include "gpumeshloader.h"

#include "meshloader.h"

#include "Meta/keyvalue/metatable_impl.h"


METATABLE_BEGIN(Engine::Render::GPUMeshLoader)
METATABLE_END(Engine::Render::GPUMeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::GPUMeshLoader::ResourceType, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Render::GPUMeshLoader::ResourceType)

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


    namespace Engine
{
    namespace Render {

        GPUMeshLoader::GPUMeshLoader()
            : VirtualResourceLoaderBase({ ".fbx", ".dae" })
        {
        }

        Threading::Task<bool> GPUMeshLoader::loadImpl(GPUMeshData &mesh, ResourceDataInfo &info)
        {
            MeshLoader::HandleType handle;
            if (!co_await handle.load(info.resource()->name()))
                co_return false;
            co_return generate(mesh, *handle);
        }

        void GPUMeshLoader::unloadImpl(GPUMeshData &data)
        {
            reset(data);
        }

        void GPUMeshLoader::generateMaterials(GPUMeshData& data, const MeshData& mesh) {
            for (const MeshData::Material &mat : mesh.mMaterials) {
                GPUMeshData::Material &gpuMat = data.mMaterials.emplace_back();
                gpuMat.mName = mat.mName;
                gpuMat.mDiffuseColor = mat.mDiffuseColor;
                
                if (!mat.mDiffuseName.empty()) {
                    gpuMat.mDiffuseTexture.loadFromImage(mat.mDiffuseName, TextureType_2D, FORMAT_RGBA8);
                }

                if (!mat.mEmissiveName.empty()) {
                    gpuMat.mEmissiveTexture.loadFromImage(mat.mEmissiveName, TextureType_2D, FORMAT_RGBA8);
                }
            }
        }

    }
}