#include "opengllib.h"

#include "openglmeshloader.h"

#include "openglmeshdata.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imageloaderlib.h"
#include "imagedata.h"
#include "imageloader.h"

#include "meshdata.h"

#include "openglrendercontext.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLMeshLoader);

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader, Engine::Render::GPUMeshLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::OpenGLMeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader::ResourceType, Engine::Render::GPUMeshLoader::ResourceType)
METATABLE_END(Engine::Render::OpenGLMeshLoader::ResourceType)

namespace Engine {
namespace Render {

    OpenGLMeshLoader::OpenGLMeshLoader()
    {
    }

    void OpenGLMeshLoader::generateImpl(OpenGLMeshData &data, const MeshData &mesh)
    {
        data.mGroupSize = mesh.mGroupSize;

        data.mVertices.setData(mesh.mVertices);

        data.mVertices.bind();

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            data.mIndices.setData(mesh.mIndices);

            data.mElementCount = mesh.mIndices.size();
            data.mIndices.bind();
        }

        generateMaterials(data, mesh);

        data.mVAO = { data.mVertices, data.mIndices, mesh.mAttributeList };
    }

    bool OpenGLMeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);
        data.mAABB = mesh.mAABB;

        generateImpl(data, mesh);

        return true;
    }

    bool OpenGLMeshLoader::generate(GPUMeshData &_data, MeshData &&mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);
        data.mAABB = mesh.mAABB;

        generateImpl(data, mesh);

        return true;
    }

    void OpenGLMeshLoader::updateImpl(OpenGLMeshData &data, const MeshData &mesh)
    {
        data.mGroupSize = mesh.mGroupSize;

        data.mVertices.resize(mesh.mVertices.mSize);
        std::memcpy(data.mVertices.mapData().mData, mesh.mVertices.mData, mesh.mVertices.mSize);

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            data.mIndices.resize(mesh.mIndices.size() * sizeof(unsigned short));
            std::memcpy(data.mIndices.mapData().mData, mesh.mIndices.data(), mesh.mIndices.size() * sizeof(unsigned short));
            data.mElementCount = mesh.mIndices.size();
        }
    }

    void OpenGLMeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mAABB = mesh.mAABB;

        updateImpl(data, mesh);
    }

    void OpenGLMeshLoader::update(GPUMeshData &_data, MeshData &&mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mAABB = mesh.mAABB;

        updateImpl(data, mesh);
    }

    void OpenGLMeshLoader::reset(GPUMeshData &data)
    {
        static_cast<OpenGLMeshData &>(data).reset();
    }

}
}
