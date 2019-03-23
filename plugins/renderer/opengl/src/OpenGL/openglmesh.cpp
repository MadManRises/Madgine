#include "opengllib.h"

#include "openglmesh.h"

#include "openglmeshloader.h"

namespace Engine {
	ENTITYCOMPONENTVIRTUALIMPL_IMPL(Render::OpenGLMesh, Scene::Entity::Mesh);

	namespace Render {		

		OpenGLMeshData *OpenGLMesh::data() const
		{
			return mData.get();
		}

		std::string OpenGLMesh::getName() const
		{
			return std::string();
		}

		void OpenGLMesh::setName(const std::string & name)
		{
			mData = OpenGLMeshLoader::load(name);
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