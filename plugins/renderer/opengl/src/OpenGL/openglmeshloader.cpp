#include "opengllib.h"

#include "openglmeshloader.h"

#include "openglmeshdata.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Modules/render/vertex.h"

#include "imageloaderlib.h"
#include "imageloader.h"
#include "imagedata.h"

namespace Engine {
namespace Render {

    OpenGLMeshLoader::OpenGLMeshLoader()
        : ResourceLoader({ ".fbx" })
    {
    }

    template <typename V>
    std::shared_ptr<OpenGLMeshData> loadVertices(const aiScene *scene, const Filesystem::Path &texturePath)
    {
        std::vector<V> vertices;
        std::vector<unsigned short> indices;
        //Reserve??

        for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            unsigned short baseVertexIndex = vertices.size();
            aiMesh *mesh = scene->mMeshes[meshIndex];

            aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

            Vector4 diffuseColor = Vector4::UNIT_SCALE;

            if constexpr (std::is_same_v<V, Vertex>) {
                aiColor3D aiDiffuseColor;
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor)) {
                    diffuseColor = { aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b, 1.0f };
                }
            }

            for (size_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
                V &vertex = vertices.emplace_back();
                aiVector3D &v = mesh->mVertices[vertexIndex];
                vertex.mPos = { v.x, v.z, v.y };
                aiVector3D &n = mesh->mNormals[vertexIndex];
                vertex.mNormal = { n.x, n.z, n.y };
                if constexpr (std::is_same_v<V, Vertex>) {
                    aiColor4D *c = mesh->mColors[0];
                    vertex.mColor = c ? Vector4 { c->r, c->g, c->b, c->a } : diffuseColor;
                }
                if constexpr (std::is_same_v<V, Vertex3>) {
                    aiVector3D &uv = mesh->mTextureCoords[0][vertexIndex];
                    vertex.mUV = { uv.x, uv.y };
                }
            }

            for (size_t i = 0; i < mesh->mNumFaces; ++i) {
                aiFace &face = mesh->mFaces[i];

                unsigned short v0 = face.mIndices[0];
                unsigned short lastIndex = face.mIndices[1];

                for (size_t j = 2; j < face.mNumIndices; ++j) {
                    unsigned short index = face.mIndices[j];

                    indices.push_back(v0 + baseVertexIndex);
                    indices.push_back(lastIndex + baseVertexIndex);
                    indices.push_back(index + baseVertexIndex);
                    lastIndex = index;
                }
            }
        }

        std::shared_ptr<OpenGLMeshData> data = std::make_shared<OpenGLMeshData>(OpenGLMeshLoader::generate(3, vertices.data(), vertices.size(), indices.data(), indices.size()));

        if (!texturePath.empty()) {
            std::string imageName = texturePath.stem();
            std::shared_ptr<Resources::ImageData> tex = Resources::ImageLoader::load(imageName);

			data->mTexture = {};
            data->mTexture.setFilter(GL_NEAREST);
            data->mTexture.setData({ tex->mWidth, tex->mHeight }, tex->mBuffer, GL_UNSIGNED_BYTE);
        }

        return data;
    }

    std::shared_ptr<OpenGLMeshData> OpenGLMeshLoader::loadImpl(ResourceType *res)
    {
        Assimp::Importer importer;

		std::vector<unsigned char> buffer = res->readAsBlob();

        const aiScene *scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), 0);

        if (!scene) {
            LOG_ERROR(importer.GetErrorString());
            return {};
        }

        if (scene->mNumMeshes == 0) {
            LOG_ERROR("No mesh in file '" << res->path().str() << "'");
            return {};
        }

        Filesystem::Path texturePath;
        for (size_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
            aiMaterial *mat = scene->mMaterials[materialIndex];
            size_t count = mat->GetTextureCount(aiTextureType_DIFFUSE);
            for (size_t i = 0; i < count; ++i) {
                aiString tempTexturePath;
                if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &tempTexturePath) != AI_SUCCESS)
                    std::terminate();
                if (tempTexturePath.length > 0) {
                    if (!texturePath.empty() && texturePath != tempTexturePath.C_Str())
                        throw "Only one texture is allowed at the moment!";
                    texturePath = tempTexturePath.C_Str();
                }
            }
        }

        if (texturePath.empty()) {
            return loadVertices<Vertex>(scene, texturePath);
        } else {
            return loadVertices<Vertex3>(scene, texturePath);
        }
    }

    OpenGLMeshData OpenGLMeshLoader::generate(size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount)
    {

        OpenGLMeshData data;
        data.mVAO.bind();
        if (indices) {
            data.mIndices = {};
            data.mIndices.bind(GL_ELEMENT_ARRAY_BUFFER);
        }

        data.mVertices.bind(GL_ARRAY_BUFFER);

        data.mVAO.enableVertexAttribute(0, &Vertex::mPos);
        data.mVAO.disableVertexAttribute(1);
        data.mVAO.enableVertexAttribute(2, &Vertex::mColor);
        data.mVAO.enableVertexAttribute(3, &Vertex::mNormal);
        data.mVAO.disableVertexAttribute(4);

        update(data, groupSize, vertices, vertexCount, indices, indexCount);

        return data;
    }

    OpenGLMeshData OpenGLMeshLoader::generate(size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount)
    {

        OpenGLMeshData data;
        data.mVAO.bind();
        if (indices) {
            data.mIndices = {};
            data.mIndices.bind(GL_ELEMENT_ARRAY_BUFFER);
        }

        data.mVertices.bind(GL_ARRAY_BUFFER);

        data.mVAO.enableVertexAttribute(0, &Vertex2::mPos);
        data.mVAO.enableVertexAttribute(1, &Vertex2::mPos2);
        data.mVAO.enableVertexAttribute(2, &Vertex2::mColor);
        data.mVAO.disableVertexAttribute(3);
        data.mVAO.enableVertexAttribute(4, &Vertex2::mUV);

        update(data, groupSize, vertices, vertexCount, indices, indexCount);

        return data;
    }

    OpenGLMeshData OpenGLMeshLoader::generate(size_t groupSize, Vertex3 *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount)
    {

        OpenGLMeshData data;
        data.mVAO.bind();
        if (indices) {
            data.mIndices = {};
            data.mIndices.bind(GL_ELEMENT_ARRAY_BUFFER);
        }

        data.mVertices.bind(GL_ARRAY_BUFFER);

        data.mVAO.enableVertexAttribute(0, &Vertex3::mPos);
        data.mVAO.disableVertexAttribute(1);
        data.mVAO.disableVertexAttribute(2);
        data.mVAO.enableVertexAttribute(3, &Vertex3::mNormal);
        data.mVAO.enableVertexAttribute(4, &Vertex3::mUV);

        update(data, groupSize, vertices, vertexCount, indices, indexCount);

        return data;
    }

    void OpenGLMeshLoader::update(OpenGLMeshData &data, size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount)
    {
        data.mGroupSize = groupSize;
        data.mVertices.setData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertexCount, vertices);

        Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

        for (size_t i = 0; i < vertexCount; ++i) {
            Vertex &v = vertices[i];
            minP = min(v.mPos, minP);
            maxP = max(v.mPos, maxP);
        }
        data.mAABB = { minP,
            maxP };

        if (indices) {
            data.mIndices.setData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indexCount, indices);
            data.mElementCount = indexCount;
        } else {
            data.mElementCount = vertexCount;
        }
    }

    void OpenGLMeshLoader::update(OpenGLMeshData &data, size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount)
    {
        data.mGroupSize = groupSize;
        data.mVertices.setData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertexCount, vertices);

        Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

        for (size_t i = 0; i < vertexCount; ++i) {
            Vertex2 &v = vertices[i];
            minP = min(v.mPos, minP);
            maxP = max(v.mPos, maxP);
        }
        data.mAABB = { minP,
            maxP };

        if (indices) {
            data.mIndices.setData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indexCount, indices);
            data.mElementCount = indexCount;
        } else {
            data.mElementCount = vertexCount;
        }
    }

    void OpenGLMeshLoader::update(OpenGLMeshData &data, size_t groupSize, Vertex3 *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount)
    {
        data.mGroupSize = groupSize;
        data.mVertices.setData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertexCount, vertices);

        Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

        for (size_t i = 0; i < vertexCount; ++i) {
            Vertex3 &v = vertices[i];
            minP = min(v.mPos, minP);
            maxP = max(v.mPos, maxP);
        }
        data.mAABB = { minP,
            maxP };

        if (indices) {
            data.mIndices.setData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indexCount, indices);
            data.mElementCount = indexCount;
        } else {
            data.mElementCount = vertexCount;
        }
    }

}
}

UNIQUECOMPONENT(Engine::Render::OpenGLMeshLoader);

using LoaderImpl = Engine::Resources::ResourceLoaderImpl<Engine::Render::OpenGLMeshData, Engine::Resources::ThreadLocalResource>;
METATABLE_BEGIN(LoaderImpl)
MEMBER(mResources)
METATABLE_END(LoaderImpl)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader, LoaderImpl)
METATABLE_END(Engine::Render::OpenGLMeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLMeshLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::OpenGLMeshLoader::ResourceType)

RegisterType(Engine::Render::OpenGLMeshLoader);