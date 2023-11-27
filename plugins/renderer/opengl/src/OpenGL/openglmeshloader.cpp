#include "opengllib.h"

#include "openglmeshloader.h"

#include "openglmeshdata.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/meshloader/meshdata.h"

#include "openglrendercontext.h"

UNIQUECOMPONENT(Engine::Render::OpenGLMeshLoader);

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader, Engine::Render::GPUMeshLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::OpenGLMeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader::Resource, Engine::Render::GPUMeshLoader::Resource)
METATABLE_END(Engine::Render::OpenGLMeshLoader::Resource)

namespace Engine {
namespace Render {

    OpenGLMeshLoader::OpenGLMeshLoader()
    {
        getOrCreateManual("quad", {}, {}, this);
        getOrCreateManual("Cube", {}, {}, this);
        getOrCreateManual("Plane", {}, {}, this);
    }

    Threading::Task<bool> OpenGLMeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mVertices.setData(mesh.mVertices);

        if (!mesh.mIndices.empty())
            data.mIndices.setData(mesh.mIndices);

        co_return co_await GPUMeshLoader::generate(data, mesh);
    }

    void OpenGLMeshLoader::reset(GPUMeshData &data)
    {
        static_cast<OpenGLMeshData &>(data).mVertices.reset();
        static_cast<OpenGLMeshData &>(data).mIndices.reset();
        GPUMeshLoader::reset(data);
    }

    UniqueResourceBlock OpenGLMeshLoader::createResourceBlock(std::vector<const Texture*> textures)
    {
        return OpenGLRenderContext::getSingleton().createResourceBlock(std::move(textures));
    }

    void OpenGLMeshLoader::destroyResourceBlock(UniqueResourceBlock &block)
    {
        OpenGLRenderContext::getSingleton().destroyResourceBlock(block);
    }

    Threading::TaskQueue *OpenGLMeshLoader::loadingTaskQueue() const
    {
        return OpenGLRenderContext::renderQueue();
    }

}
}
