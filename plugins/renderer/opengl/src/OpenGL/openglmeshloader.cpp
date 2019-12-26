#include "opengllib.h"

#include "openglmeshloader.h"

#include "openglmeshdata.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "imageloaderlib.h"
#include "imagedata.h"
#include "imageloader.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLMeshLoader);

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader, Engine::Resources::MeshLoader)
METATABLE_END(Engine::Render::OpenGLMeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader::ResourceType, Engine::Resources::MeshLoader::ResourceType)
METATABLE_END(Engine::Render::OpenGLMeshLoader::ResourceType)

RegisterType(Engine::Render::OpenGLMeshLoader);

namespace Engine {
namespace Render {

    OpenGLMeshLoader::OpenGLMeshLoader()
    {
    }

    bool OpenGLMeshLoader::generateImpl(Resources::MeshData &_data, const std::vector<std::optional<AttributeDescriptor>> &attributeList, const AABB &bb, size_t groupSize, void *vertices, size_t vertexCount, size_t vertexSize, unsigned short *indices, size_t indexCount, const Filesystem::Path &texturePath)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);
        data.mVAO = create;
        data.mVAO.bind();

		data.mVertices = create;
        data.mVertices.bind(GL_ARRAY_BUFFER);

        if (indices) {
            data.mIndices = create;
        }

        if (!texturePath.empty()) {
            std::string imageName = texturePath.stem();
            Resources::ImageLoader::HandleType tex;
            tex.load(imageName);

            data.mTexture = { GL_UNSIGNED_BYTE };
            data.mTexture.setFilter(GL_NEAREST);
            data.mTexture.setData({ tex->mWidth, tex->mHeight }, tex->mBuffer);
            data.mTextureHandle = data.mTexture.handle();
        }

        updateImpl(data, bb, groupSize, vertices, vertexCount, vertexSize, indices, indexCount);

        for (int i = 0; i < 5; ++i) {
            if (attributeList[i])
                data.mVAO.enableVertexAttribute(i, *attributeList[i]);
            else
                data.mVAO.disableVertexAttribute(i);
        }

        return true;
    }

    void OpenGLMeshLoader::updateImpl(Resources::MeshData &_data, const AABB &bb, size_t groupSize, const void *vertices, size_t vertexCount, size_t vertexSize, unsigned short *indices, size_t indexCount)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mAABB = bb;

        data.mGroupSize = groupSize;
        data.mVertices.setData(GL_ARRAY_BUFFER, vertexSize * vertexCount, vertices);

        if (indices) {
            data.mIndices.setData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indexCount, indices);
            data.mElementCount = indexCount;
        } else {
            data.mElementCount = vertexCount;
        }
    }

    void OpenGLMeshLoader::resetImpl(Resources::MeshData &data)
    {
        static_cast<OpenGLMeshData &>(data).reset();
    }

}
}
