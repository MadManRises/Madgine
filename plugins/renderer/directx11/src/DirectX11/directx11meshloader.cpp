#include "directx11lib.h"

#include "directx11meshloader.h"

#include "directx11meshdata.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "imageloaderlib.h"
#include "imagedata.h"
#include "imageloader.h"

#include "directx11rendercontext.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX11MeshLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX11MeshLoader, Engine::Render::MeshLoader)
METATABLE_END(Engine::Render::DirectX11MeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11MeshLoader::ResourceType, Engine::Render::MeshLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX11MeshLoader::ResourceType)

RegisterType(Engine::Render::DirectX11MeshLoader);

namespace Engine {
namespace Render {

    DirectX11MeshLoader::DirectX11MeshLoader()
    {
    }

    bool DirectX11MeshLoader::generateImpl(MeshData &_data, const std::vector<std::optional<AttributeDescriptor>> &attributeList, const AABB &bb, size_t groupSize, ByteBuffer vertices, size_t vertexSize, std::vector<unsigned short> indices, const Filesystem::Path &texturePath)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);
        data.mAABB = bb;

        DirectX11RenderContext::execute([=, &data, vertices { std::move(vertices) }, indices { std::move(indices) }]() mutable {
            data.mGroupSize = groupSize;
            
            data.mVAO = { attributeList };
            data.mVAO.bind();

            data.mVertices = {
                D3D11_BIND_VERTEX_BUFFER,
                vertices.mSize,
                vertices.mData
            };
            data.mVertices.bindVertex(data.mVAO.mStride);
            
            if (indices.empty()) {
                data.mElementCount = vertices.mSize / vertexSize;
            }else{
                data.mIndices = {
                    D3D11_BIND_INDEX_BUFFER,
                    indices.size() * sizeof(unsigned short),
                    indices.data()
                };
                data.mIndices.bindIndex();
                data.mElementCount = indices.size();
            }

            if (!texturePath.empty()) {
                std::string imageName = texturePath.stem();
                Resources::ImageLoader::HandleType tex;
                tex.load(imageName);

                data.mTexture = { Texture2D, FORMAT_FLOAT8, D3D11_BIND_SHADER_RESOURCE, static_cast<size_t>(tex->mWidth), static_cast<size_t>(tex->mHeight), tex->mBuffer };
                data.mTextureHandle = data.mTexture.handle();
            }

            data.mVAO.unbind();
        });

        return true;
    }

    void DirectX11MeshLoader::updateImpl(MeshData &_data, const AABB &bb, size_t groupSize, ByteBuffer vertices, size_t vertexSize, std::vector<unsigned short> indices)
    {
        DirectX11MeshData &data = static_cast<DirectX11MeshData &>(_data);

        data.mAABB = bb;

        DirectX11RenderContext::execute([=, &data, vertices { std::move(vertices) }, indices { std::move(indices) }]() mutable {
            data.mGroupSize = groupSize;

            data.mVertices = {
                D3D11_BIND_VERTEX_BUFFER,
                vertices.mSize,
                vertices.mData
            };

            if (indices.empty()) {
                data.mElementCount = vertices.mSize / vertexSize;
            }else{
                data.mIndices = {
                    D3D11_BIND_INDEX_BUFFER,
                    indices.size() * sizeof(unsigned short),
                    indices.data()
                };
                data.mElementCount = indices.size();
            } 
        });
    }

    void DirectX11MeshLoader::resetImpl(MeshData &data)
    {
        static_cast<DirectX11MeshData &>(data).reset();
    }

}
}
