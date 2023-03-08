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
        getOrCreateManual("Cube", {}, {}, this);
        getOrCreateManual("Plane", {}, {}, this);
    }

    bool OpenGLMeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mVertices.setData(mesh.mVertices);

        if (!mesh.mIndices.empty())
            data.mIndices.setData(mesh.mIndices);

        return GPUMeshLoader::generate(data, mesh);
    }

    void OpenGLMeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mVertices.resize(mesh.mVertices.mSize);
        std::memcpy(data.mVertices.mapData().mData, mesh.mVertices.mData, mesh.mVertices.mSize);

        if (!mesh.mIndices.empty()) {
            data.mIndices.resize(mesh.mIndices.size() * sizeof(uint32_t));
            std::memcpy(data.mIndices.mapData().mData, mesh.mIndices.data(), mesh.mIndices.size() * sizeof(uint32_t));
        }

        GPUMeshLoader::update(data, mesh);
    }

    void OpenGLMeshLoader::reset(GPUMeshData &data)
    {
        static_cast<OpenGLMeshData &>(data).reset();
    }

        Threading::TaskQueue *OpenGLMeshLoader::loadingTaskQueue() const
    {
        return OpenGLRenderContext::renderQueue();
    }

}
}
