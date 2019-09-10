#include "opengllib.h"

#include "openglmeshloader.h"

#include "openglmeshdata.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Madgine/render/vertex.h"

namespace Engine {
namespace Render {

    OpenGLMeshLoader::OpenGLMeshLoader()
        : ResourceLoader({ ".fbx" })
    {
    }

    std::shared_ptr<OpenGLMeshData> OpenGLMeshLoader::loadImpl(ResourceType *res)
    {
        Assimp::Importer importer;

        const aiScene *scene = importer.ReadFile(res->path().str(), 0);

        if (!scene) {
            LOG_ERROR(importer.GetErrorString());
            return {};
        }

        if (scene->mNumMeshes == 0) {
            LOG_ERROR("No mesh in file '" << res->path().str() << "'");
            return {};
        }

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        //Reserve??

        for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            unsigned int baseVertexIndex = vertices.size();
            aiMesh *mesh = scene->mMeshes[meshIndex];

            aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

            aiColor3D aiDiffuseColor;
            Vector4 diffuseColor = Vector4::UNIT_SCALE;
            if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor)) {
                diffuseColor = { aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b, 1.0f };
            }

            for (size_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
                Vertex &vertex = vertices.emplace_back();
                aiVector3D &v = mesh->mVertices[vertexIndex];
                vertex.mPos = { v.x, v.z, v.y };
                aiVector3D &n = mesh->mNormals[vertexIndex];
                vertex.mNormal = { n.x, n.z, n.y };
                aiColor4D *c = mesh->mColors[0];
                vertex.mColor = c ? Vector4 { c->r, c->g, c->b, c->a } : diffuseColor;
            }

            for (size_t i = 0; i < mesh->mNumFaces; ++i) {
                aiFace &face = mesh->mFaces[i];

                unsigned int v0 = face.mIndices[0];
                unsigned int lastIndex = face.mIndices[1];

                for (size_t j = 2; j < face.mNumIndices; ++j) {
                    unsigned int index = face.mIndices[j];

                    indices.push_back(v0 + baseVertexIndex);
                    indices.push_back(lastIndex + baseVertexIndex);
                    indices.push_back(index + baseVertexIndex);
                    lastIndex = index;
                }
            }
        }

        return std::make_shared<OpenGLMeshData>(generate(3, vertices.data(), vertices.size(), indices.data(), indices.size()));
    }

    OpenGLMeshData OpenGLMeshLoader::generate(size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned int *indices, size_t indexCount)
    {

        OpenGLMeshData data;
        data.mVAO.bind();
        if (indices) {
            data.mIndices = {};
            data.mIndices.bind(GL_ELEMENT_ARRAY_BUFFER);
        }

        data.mVertices.bind(GL_ARRAY_BUFFER);

        glVertexAttribPointer(
            0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Vertex), // stride
            (void *)offsetof(Vertex, mPos) // array buffer offset
        );
        glEnableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glVertexAttribPointer(
            2, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            4, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Vertex), // stride
            (void *)offsetof(Vertex, mColor) // array buffer offset
        );
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            3, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Vertex), // stride
            (void *)offsetof(Vertex, mNormal) // array buffer offset
        );
        glEnableVertexAttribArray(3);
        glDisableVertexAttribArray(4);

        update(data, groupSize, vertices, vertexCount, indices, indexCount);

        return data;
    }

    OpenGLMeshData OpenGLMeshLoader::generate(size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned int *indices, size_t indexCount)
    {

        OpenGLMeshData data;
        data.mVAO.bind();
        if (indices) {
            data.mIndices = {};
            data.mIndices.bind(GL_ELEMENT_ARRAY_BUFFER);
        }

        data.mVertices.bind(GL_ARRAY_BUFFER);

        glVertexAttribPointer(
            0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Vertex2), // stride
            (void *)offsetof(Vertex2, mPos) // array buffer offset
        );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            1, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Vertex2), // stride
            (void *)offsetof(Vertex2, mPos2) // array buffer offset
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            2, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            4, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Vertex2), // stride
            (void *)offsetof(Vertex2, mColor) // array buffer offset
        );
        glEnableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
		glVertexAttribPointer(
            4, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Vertex2), // stride
            (void *)offsetof(Vertex2, mUV) // array buffer offset
        );
        glEnableVertexAttribArray(4);       


        update(data, groupSize, vertices, vertexCount, indices, indexCount);

        return data;
    }

    void OpenGLMeshLoader::update(OpenGLMeshData &data, size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned int *indices, size_t indexCount)
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

    void OpenGLMeshLoader::update(OpenGLMeshData &data, size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned int *indices, size_t indexCount)
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

}
}

UNIQUECOMPONENT(Engine::Render::OpenGLMeshLoader);

METATABLE_BEGIN(Engine::Render::OpenGLMeshLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::OpenGLMeshLoader)

METATABLE_BEGIN(Engine::Render::OpenGLMeshLoader::ResourceType)
METATABLE_END(Engine::Render::OpenGLMeshLoader::ResourceType)

RegisterType(Engine::Render::OpenGLMeshLoader);