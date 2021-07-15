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

        data.mVAO = Render::create;
        data.mVAO.bind();

        bool dynamic = !mesh.mVertices.mData;

        if (!dynamic) {
            data.mVertices = {
                GL_ARRAY_BUFFER,
                mesh.mVertices
            };
        } else {
            data.mVertices = {
                GL_ARRAY_BUFFER,
                mesh.mVertices.mSize
            };
        }

        if (mesh.mIndices.empty()) {
            data.mElementCount = mesh.mVertices.mSize / mesh.mVertexSize;
        } else {
            if (!dynamic) {
                data.mIndices = {
                    GL_ELEMENT_ARRAY_BUFFER,
                    mesh.mIndices
                };
            } else {
                data.mIndices = {
                    GL_ELEMENT_ARRAY_BUFFER,
                    mesh.mIndices.size() * sizeof(unsigned short)
                };
            }
            data.mElementCount = mesh.mIndices.size();
        }

        if (!mesh.mTexturePath.empty()) {
            std::string_view imageName = mesh.mTexturePath.stem();
            Resources::ImageLoader::HandleType tex;
            tex.load(imageName);

            if (tex) {
                data.mTexture = { GL_UNSIGNED_BYTE };
                //data.mTexture.setFilter(GL_NEAREST);
                data.mTexture.setData({ tex->mWidth, tex->mHeight }, { tex->mBuffer, static_cast<size_t>(tex->mWidth * tex->mHeight) });
                data.mTextureHandle = data.mTexture.mTextureHandle;
            } else {
                LOG_ERROR("Failed to find texture '" << imageName << "' for mesh!");
            }
        }

        auto attributes = mesh.mAttributeList();

        for (int i = 0; i < attributes.size(); ++i) {
            data.mVAO.setVertexAttribute(i, attributes[i]);
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
