#include "meshloaderlib.h"

#include "gpumeshloader.h"

#include "meshloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "render/vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "../assimptools.h"

#include "Meta/math/transformation.h"

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
METATABLE_END(Engine::Render::GPUMeshData::Material)


    namespace Engine
{
    namespace Render {

        GPUMeshLoader::GPUMeshLoader()
            : VirtualResourceLoaderBase({ ".fbx", ".dae" })
        {
        }

        bool GPUMeshLoader::loadImpl(GPUMeshData &mesh, ResourceDataInfo &info)
        {
            return generate(mesh, *MeshLoader::load(info.resource()->name()));
        }

        void GPUMeshLoader::unloadImpl(GPUMeshData &data, ResourceDataInfo &info)
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
                    gpuMat.mDiffuseHandle = gpuMat.mDiffuseTexture->mTextureHandle;
                }

                if (!mat.mEmissiveName.empty()) {
                    gpuMat.mEmissiveTexture.loadFromImage(mat.mEmissiveName, TextureType_2D, FORMAT_RGBA8);
                    gpuMat.mEmissiveHandle = gpuMat.mEmissiveTexture->mTextureHandle;
                }
            }
        }

    }
}