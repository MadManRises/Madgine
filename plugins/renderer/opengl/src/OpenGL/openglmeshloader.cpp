#include "opengllib.h"

#include "openglmeshloader.h"

#include "openglmeshdata.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::Render::OpenGLMeshLoader);

namespace Engine
{
	namespace Render 
	{

		OpenGLMeshLoader::OpenGLMeshLoader(Resources::ResourceManager & mgr) :
			ResourceLoader(mgr, { ".fbx" })
		{
		}

		std::shared_ptr<OpenGLMeshData> OpenGLMeshLoader::loadImpl(ResourceType * res)
		{
			Assimp::Importer importer;

			const aiScene *scene = importer.ReadFile(res->path().str(), 0);

			if (!scene)
			{
				LOG_ERROR(importer.GetErrorString());
				return {};
			}

			if (scene->mNumMeshes == 0)
			{
				LOG_ERROR("No mesh in file '" << res->path().str() << "'");
				return {};
			}

			std::vector<Vertex> vertices;
			//data->mVertices.resize(mesh->mNumFaces * 3);

			for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
			{
				aiMesh *mesh = scene->mMeshes[meshIndex];

				aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

				aiColor3D aiDiffuseColor;
				Vector4 diffuseColor = Vector4::UNIT_SCALE;
				if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor)) {					
					diffuseColor = { aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b, 1.0f };
				}	

				for (size_t i = 0; i < mesh->mNumFaces; ++i)
				{
					aiFace &face = mesh->mFaces[i];

					auto toVertex = [&](size_t j) {
						Vertex result;
						size_t index = face.mIndices[j];
						aiVector3D &v = mesh->mVertices[index];
						result.mPos = { v.x, v.z, v.y };
						aiVector3D &n = mesh->mNormals[index];
						result.mNormal = { n.x, n.z, n.y };
						aiColor4D *c = mesh->mColors[0];
						result.mColor = c ? Vector4{ c->r, c->g, c->b, c->a } : diffuseColor;
						return result;
					};

					Vertex v0 = toVertex(0);
					Vertex lastVertex = toVertex(1);

					for (size_t j = 2; j < face.mNumIndices; ++j)
					{
						Vertex v = toVertex(j);
						vertices.push_back(v0);
						vertices.push_back(lastVertex);
						vertices.push_back(v);
						lastVertex = v;
					}
				}
			}

			std::shared_ptr<OpenGLMeshData> data = std::make_shared<OpenGLMeshData>();
			
			glGenVertexArrays(1, &data->mVAO);
			glBindVertexArray(data->mVAO);

			data->mVertices.bind(GL_ARRAY_BUFFER);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(Vertex),                  // stride
				(void*)0            // array buffer offset
			);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(Vertex),                  // stride
				(void*)12            // array buffer offset
			);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(Vertex),                  // stride
				(void*)28            // array buffer offset
			);
			glEnableVertexAttribArray(2);


			data->mVertices.setData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data());
			data->mVertexCount = vertices.size();

			return data;
		}

	}
}

METATABLE_BEGIN(Engine::Render::OpenGLMeshLoader)
METATABLE_END(Engine::Render::OpenGLMeshLoader)

RegisterType(Engine::Render::OpenGLMeshLoader);