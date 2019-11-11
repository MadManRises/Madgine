#include "opengllib.h"

#include "openglmeshloader.h"

#include "openglmeshdata.h"


#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"


#include "imageloaderlib.h"
#include "imageloader.h"
#include "imagedata.h"


VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLMeshLoader);

using LoaderImpl = Engine::Resources::ResourceLoaderImpl<Engine::Resources::MeshData, Engine::Resources::ThreadLocalResource>;
METATABLE_BEGIN(LoaderImpl)
MEMBER(mResources)
METATABLE_END(LoaderImpl)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader, LoaderImpl)
METATABLE_END(Engine::Render::OpenGLMeshLoader)

RegisterType(Engine::Render::OpenGLMeshLoader);

namespace Engine {
namespace Render {

    OpenGLMeshLoader::OpenGLMeshLoader()
    {
    }



    std::shared_ptr<Resources::MeshData> OpenGLMeshLoader::generateImpl(const std::vector<std::optional<AttributeDescriptor>> &attributeList, const AABB &bb, size_t groupSize, void *vertices, size_t vertexCount, size_t vertexSize, unsigned short *indices, size_t indexCount, const Filesystem::Path &texturePath)
    {
        std::shared_ptr<OpenGLMeshData> data = std::make_shared<OpenGLMeshData>();
        data->mVAO.bind();
        
		data->mVertices.bind(GL_ARRAY_BUFFER);

		if (indices) {
            data->mIndices = {};
        }

		if (!texturePath.empty()) {
            std::string imageName = texturePath.stem();
            std::shared_ptr<Resources::ImageData> tex = Resources::ImageLoader::load(imageName);

            OpenGLMeshData &_data = static_cast<OpenGLMeshData &>(*data);

            _data.mTexture = {GL_UNSIGNED_BYTE};
            _data.mTexture.setFilter(GL_NEAREST);
            _data.mTexture.setData({ tex->mWidth, tex->mHeight }, tex->mBuffer);
            data->mTextureHandle = _data.mTexture.handle();
        }

        updateImpl(*data, bb, groupSize, vertices, vertexCount, vertexSize, indices, indexCount);

        for (int i = 0; i < 5; ++i) {
            if (attributeList[i])
                data->mVAO.enableVertexAttribute(i, *attributeList[i]);
            else
                data->mVAO.disableVertexAttribute(i);
        }

        return data;
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

}
}
