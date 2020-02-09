#include "meshloaderlib.h"

#include "meshloader.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Modules/render/vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "../assimptools.h"

#include "Modules/math/transformation.h"

METATABLE_BEGIN(Engine::Render::MeshLoader)
METATABLE_END(Engine::Render::MeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::MeshLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::MeshLoader::ResourceType)

RegisterType(Engine::Render::MeshLoader)

    namespace Engine
{
    namespace Render {

        MeshLoader::MeshLoader()
            : VirtualResourceLoaderBase({ ".fbx", ".dae" })
        {
        }

        template <typename V>
        bool loadVertices(MeshData &mesh, MeshLoader &loader, const aiScene *scene, const Filesystem::Path &texturePath)
        {
            std::vector<V> vertices;
            std::vector<unsigned short> indices;
            //Reserve??

            assimpTraverseTree(scene, [&](const aiNode *node, const Matrix4 &transform) {
                for (size_t meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex) {
                    unsigned short baseVertexIndex = vertices.size();
                    aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];

                    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

                    Vector4 diffuseColor = Vector4::UNIT_SCALE;
                    Vector4 ambientColor = Vector4::UNIT_SCALE;

                    if constexpr (std::is_same_v<V, Render::Vertex>) {
                        aiColor3D aiDiffuseColor;
                        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor)) {
                            diffuseColor = { aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b, 1.0f };
                        }
                        aiColor3D aiAmbientColor;
                        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbientColor)) {
                            ambientColor = { aiAmbientColor.r, aiAmbientColor.g, aiAmbientColor.b, 1.0f };
                        }
                    }

                    for (size_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
                        V &vertex = vertices.emplace_back();
                        aiVector3D v = mesh->mVertices[vertexIndex];
                        vertex.mPos = transform.ToMat3() * Vector3 { v.x, v.y, v.z };
                        aiVector3D n = mesh->mNormals[vertexIndex];
                        vertex.mNormal = transform.ToMat3().Inverse().Transpose() * Vector3 { n.x, n.y, n.z };
                        if constexpr (std::is_same_v<V, Render::Vertex>) {
                            aiColor4D *c = mesh->mColors[0];
                            vertex.mColor = c ? Vector4 { c->r, c->g, c->b, c->a } : diffuseColor;
                        }
                        if constexpr (std::is_same_v<V, Render::Vertex3>) {
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
                },
                Matrix4 { ScalingMatrix(0.01 * Vector3 { Vector3::UNIT_SCALE }) });

            return loader.generate(mesh, 3, std::move(vertices), std::move(indices), texturePath);
        }

        bool MeshLoader::loadImpl(MeshData &mesh, ResourceType *res)
        {
            Assimp::Importer importer;

            std::vector<unsigned char> buffer = res->readAsBlob();

            const aiScene *scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), 0);

            if (!scene) {
                LOG_ERROR(importer.GetErrorString());
                return false;
            }

            if (scene->mNumMeshes == 0) {
                LOG_ERROR("No mesh in file '" << res->path().str() << "'");
                return false;
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
                return loadVertices<Render::Vertex>(mesh, *this, scene, texturePath);
            } else {
                return loadVertices<Render::Vertex3>(mesh, *this, scene, texturePath);
            }
        }

        void MeshLoader::unloadImpl(MeshData &data, ResourceType *res)
        {
            resetImpl(data);
        }

    }
}