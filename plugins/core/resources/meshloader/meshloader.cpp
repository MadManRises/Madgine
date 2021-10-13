#include "meshloaderlib.h"

#include "meshloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "render/vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../assimptools.h"

#include "Meta/math/transformation.h"

METATABLE_BEGIN(Engine::Render::MeshLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::MeshLoader)

METATABLE_BEGIN_BASE(Engine::Render::MeshLoader::ResourceType, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Render::MeshLoader::ResourceType)

UNIQUECOMPONENT(Engine::Render::MeshLoader)

    namespace Engine
{
    namespace Render {

        MeshLoader::MeshLoader()
            : ResourceLoader({ ".fbx", ".dae" })
        {
        }

        template <typename V>
        bool loadVertices(MeshData &mesh, MeshLoader &loader, const aiScene *scene, std::vector<MeshData::Material> materials)
        {
            std::vector<V> vertices;
            std::vector<unsigned short> indices;
            //Reserve??

			std::map<aiNode*, size_t> boneIndices;

            for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
                aiMesh *mesh = scene->mMeshes[meshIndex];

                for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                    aiBone *bone = mesh->mBones[boneIndex];
                    aiNode *node = scene->mRootNode->FindNode(bone->mName);

                    boneIndices.try_emplace(node, boneIndices.size());
                }
            }

            assimpTraverseTree(
                scene, [&](const aiNode *node, const Matrix4 &t) {
                    Matrix3 transform_anti = t.ToMat3().Inverse().Transpose();

                    for (size_t meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex) {
                        unsigned short baseVertexIndex = vertices.size();
                        aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];

                        for (size_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
                            V &vertex = vertices.emplace_back();
                            aiVector3D v = mesh->mVertices[vertexIndex];
                            vertex.mPos = (t * Vector4 { v.x, v.y, v.z, 1.0f }).xyz();
                            if constexpr (V::template holds<VertexNormal>) {
                                aiVector3D n = mesh->mNormals[vertexIndex];
                                vertex.mNormal = transform_anti * Vector3 { n.x, n.y, n.z };
                            }
                            if constexpr (V::template holds<VertexColor>) {
                                aiColor4D *c = mesh->mColors[0];
                                vertex.mColor = c ? Vector4 { c->r, c->g, c->b, c->a } : Vector4::UNIT_SCALE;
                            }
                            if constexpr (V::template holds<VertexUV>) {
                                aiVector3D &uv = mesh->mTextureCoords[0][vertexIndex];
                                vertex.mUV = { uv.x, 1.0f - uv.y };
                            }
                        }

                        if constexpr (V::template holds<VertexBoneMappings>) {
                            for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                                aiBone *bone = mesh->mBones[boneIndex];
                                size_t actualIndex = boneIndices.at(scene->mRootNode->FindNode(bone->mName));
                                for (size_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
                                    size_t vertexIndex = baseVertexIndex + bone->mWeights[weightIndex].mVertexId;
                                    float weight = bone->mWeights[weightIndex].mWeight;
                                    VertexBoneMappings &mappings = vertices[vertexIndex];
                                    size_t freeIndex = 0;
                                    while (freeIndex < 4 && mappings.mBoneWeights[freeIndex] != 0.0f)
                                        ++freeIndex;
                                    if (freeIndex == 4) {
                                        LOG_WARNING("Vertex is assigned more than 4 Bones. That is not supported currently, animation might look wrong.");
                                    } else {
                                        mappings.mBoneIndices[freeIndex] = actualIndex;
                                        mappings.mBoneWeights[freeIndex] = weight;
                                    }
                                }
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
                });

            mesh = {3, std::move(vertices), std::move(indices), std::move(materials)};
            return true;
        }

        bool MeshLoader::loadImpl(MeshData &mesh, ResourceDataInfo &info)
        {
            Assimp::Importer importer;

            std::vector<unsigned char> buffer = info.resource()->readAsBlob();

            const aiScene *scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), aiProcess_MakeLeftHanded | aiProcess_Triangulate);

            if (!scene) {
                LOG_ERROR(importer.GetErrorString());
                return false;
            }

            if (scene->mNumMeshes == 0) {
                LOG_ERROR("No mesh in file '" << info.resource()->path().str() << "'");
                return false;
            }

            std::vector<MeshData::Material> materials;
            for (size_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
                aiMaterial *mat = scene->mMaterials[materialIndex];
                if (mat->GetName() == aiString { "DefaultMaterial" })
                    continue; //TODO: Find proper solution

                MeshData::Material &targetMat = materials.emplace_back();
                targetMat.mName = mat->GetName().C_Str();
                aiColor3D aiDiffuseColor;
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor)) {
                    targetMat.mDiffuseColor = { aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b, 1.0f };
                }
                size_t count = mat->GetTextureCount(aiTextureType_DIFFUSE);
                for (size_t i = 0; i < count; ++i) {
                    aiString diffuseTexturePath;
                    if (mat->GetTexture(aiTextureType_DIFFUSE, i, &diffuseTexturePath) == AI_SUCCESS && diffuseTexturePath.length > 0) {
                        std::string diffuseName { Filesystem::Path { diffuseTexturePath.C_Str() }.stem() };
                        if (!targetMat.mDiffuseName.empty() && targetMat.mDiffuseName != diffuseName)
                            throw "Only one diffuse texture is allowed at the moment!";
                        targetMat.mDiffuseName = diffuseName;
                    }
                }

                count = mat->GetTextureCount(aiTextureType_EMISSIVE);
                for (size_t i = 0; i < count; ++i) {
                    aiString emissiveTexturePath;
                    if (mat->GetTexture(aiTextureType_EMISSIVE, i, &emissiveTexturePath) == AI_SUCCESS && emissiveTexturePath.length > 0){
                        std::string emissiveName { Filesystem::Path { emissiveTexturePath.C_Str() }.stem() };
                        if (!targetMat.mEmissiveName.empty() && targetMat.mEmissiveName != emissiveName)
                            throw "Only one emissive texture is allowed at the moment!";
                        targetMat.mEmissiveName = emissiveName;
                    }
                }
            }

            bool hasSkeleton = false;
            assimpTraverseTree(scene, [&](const aiNode *node) {
                for (size_t i = 0; i < node->mNumMeshes; ++i) {
                    hasSkeleton |= scene->mMeshes[node->mMeshes[i]]->HasBones();
                }
            });

            if (!hasSkeleton) {
                if (materials.empty()) {
                    return loadVertices<Render::Vertex>(mesh, *this, scene, std::move(materials));
                } else {
                    return loadVertices<Render::Vertex3>(mesh, *this, scene, std::move(materials));
                }
            } else {
                if (materials.empty()) {
                    return loadVertices<Render::Vertex4>(mesh, *this, scene, std::move(materials));
                } else {
                    std::terminate();
                }
            }
        }

        void MeshLoader::unloadImpl(MeshData &data, ResourceDataInfo &info)
        {
            data.mAttributeList = nullptr;
            data.mGroupSize = 0;
            data.mIndices.clear();
            data.mMaterials.clear();
            data.mVertexSize = 0;
            data.mVertices.clear();            
        }

    }
}