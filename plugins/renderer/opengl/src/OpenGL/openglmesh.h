#include "Madgine/scene/entity/components/mesh.h"

#include "openglmeshdata.h"

namespace Engine 
{
	namespace Render {

		struct MADGINE_OPENGL_EXPORT OpenGLMesh : Scene::Entity::EntityComponentVirtualImpl<OpenGLMesh, Scene::Entity::Mesh>
		{
			using EntityComponentVirtualImpl::EntityComponentVirtualImpl;

			OpenGLMeshData *data() const;

			// Geerbt über EntityComponentVirtualImpl
			virtual std::string getName() const override;
			virtual void setName(const std::string & name) override;
			virtual void setVisible(bool vis) override;
			virtual bool isVisible() const override;

		private:
			std::shared_ptr<OpenGLMeshData> mData;
		};

	}
}