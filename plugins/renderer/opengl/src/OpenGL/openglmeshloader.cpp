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

    void OpenGLMeshLoader::generateImpl(OpenGLMeshData& data, const MeshData& mesh) {
        data.mVAO = create;
        data.mVAO.bind();

        data.mVertices = GL_ARRAY_BUFFER;

        if (!mesh.mIndices.empty()) {
            data.mIndices = GL_ELEMENT_ARRAY_BUFFER;
        }

        if (!mesh.mTexturePath.empty()) {
            std::string_view imageName = mesh.mTexturePath.stem();
            Resources::ImageLoader::HandleType tex;
            tex.load(imageName);

            if (tex) {
                data.mTexture = { GL_UNSIGNED_BYTE };
                data.mTexture.setFilter(GL_NEAREST);
                data.mTexture.setData({ tex->mWidth, tex->mHeight }, { tex->mBuffer, static_cast<size_t>(tex->mWidth * tex->mHeight) });
                data.mTextureHandle = data.mTexture.handle();
            } else {
                LOG_ERROR("Failed to find texture '" << imageName << "' for mesh!");
            }
        }

        updateImpl(data, mesh);

        for (int i = 0; i < mesh.mAttributeList.size(); ++i) {
            if (mesh.mAttributeList[i])
                data.mVAO.enableVertexAttribute(i, *mesh.mAttributeList[i]);
            else
                data.mVAO.disableVertexAttribute(i);
        }

        data.mVAO.unbind();
    }

    bool OpenGLMeshLoader::generate(GPUMeshData &_data, const MeshData &mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);
        data.mAABB = mesh.mAABB;

        OpenGLRenderContext::execute([=, &data, &mesh]() mutable {
            generateImpl(data, mesh);
        });

        return true;
    }

       bool OpenGLMeshLoader::generate(GPUMeshData &_data, MeshData &&mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);
        data.mAABB = mesh.mAABB;

        OpenGLRenderContext::execute([=, &data, mesh { std::move(mesh) }]() mutable {
            generateImpl(data, mesh);
        });

        return true;
    }

    void OpenGLMeshLoader::updateImpl(OpenGLMeshData &data, const MeshData &mesh)
    {
        data.mGroupSize = mesh.mGroupSize;

        data.mVertices.setData(mesh.mVertices);

        if (!mesh.mIndices.empty()) {
            data.mIndices.setData(mesh.mIndices);
            data.mElementCount = mesh.mIndices.size();
        } else {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        }
    }

    void OpenGLMeshLoader::update(GPUMeshData &_data, const MeshData &mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mAABB = mesh.mAABB;

        OpenGLRenderContext::execute([=, &data, &mesh]() mutable {
            updateImpl(data, mesh);
        });
    }

        void OpenGLMeshLoader::update(GPUMeshData &_data, MeshData &&mesh)
    {
        OpenGLMeshData &data = static_cast<OpenGLMeshData &>(_data);

        data.mAABB = mesh.mAABB;

        OpenGLRenderContext::execute([=, &data, mesh { std::move(mesh) }]() mutable {
            updateImpl(data, mesh);
        });
    }

    void OpenGLMeshLoader::reset(GPUMeshData &data)
    {
        static_cast<OpenGLMeshData &>(data).reset();
    }

}
}
