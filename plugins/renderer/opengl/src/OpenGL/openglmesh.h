#include "Madgine/scene/entity/components/mesh.h"

#include "openglmeshdata.h"
#include "openglmeshloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMesh : Scene::Entity::EntityComponentVirtualImpl<OpenGLMesh, Scene::Entity::Mesh> {
        OpenGLMesh(Scene::Entity::Entity &e, const ObjectPtr &init);

        OpenGLMeshData *data() const;

        // Geerbt über EntityComponentVirtualImpl
        virtual std::string getName() const override;
        virtual void setName(const std::string &name) override;
        virtual void setVisible(bool vis) override;
        virtual bool isVisible() const override;

        void setManual(std::shared_ptr<OpenGLMeshData> data);

        void set(OpenGLMeshLoader::ResourceType *mesh);
        OpenGLMeshLoader::ResourceType *get() const;

    private:
        std::shared_ptr<OpenGLMeshData> mData;
        typename OpenGLMeshLoader::ResourceType *mResource;
    };

}
}