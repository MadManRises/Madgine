#include "opengllib.h"

#include "openglmesh.h"

#include "openglmeshloader.h"

#include "Interfaces/generic/valuetype.h"

namespace Engine {
	ENTITYCOMPONENTVIRTUALIMPL_IMPL(Render::OpenGLMesh, Scene::Entity::Mesh);

	namespace Render {
		OpenGLMesh::OpenGLMesh(Scene::Entity::Entity & e, const Scripting::LuaTable & data) :
			EntityComponentVirtualImpl<OpenGLMesh, Scene::Entity::Mesh>(e, data),
			mResource(nullptr)
		{
			if (const Engine::ValueType &v = data["mesh"]; v.is<std::string>())
			{
				setName(v.as<std::string>());
			}
		}

		OpenGLMeshData *OpenGLMesh::data() const
		{
			return mData.get();
		}

		std::string OpenGLMesh::getName() const
		{
			return mResource->name();
		}

		void OpenGLMesh::setName(const std::string & name)
		{
			mResource = Resources::ResourceManager::getSingleton().get<OpenGLMeshLoader>().get(name);
			if (mResource)
				mData = mResource->loadData();
			else
				mData.reset();
		}

		void OpenGLMesh::setVisible(bool vis)
		{
		}

		bool OpenGLMesh::isVisible() const
		{
			return true;
		}

	}
}