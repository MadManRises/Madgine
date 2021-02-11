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
METATABLE_END(Engine::Render::GPUMeshLoader::ResourceType)



    namespace Engine
{
    namespace Render {

        GPUMeshLoader::GPUMeshLoader()
            : VirtualResourceLoaderBase({ ".fbx", ".dae" })
        {
        }

        bool GPUMeshLoader::loadImpl(GPUMeshData &mesh, ResourceType *res)
        {
            return generate(mesh, *MeshLoader::load(res->name()));
        }

        void GPUMeshLoader::unloadImpl(GPUMeshData &data, ResourceType *res)
        {
            reset(data);
        }

    }
}