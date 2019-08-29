#include "opengllib.h"

#include "openglmesh.h"

#include "openglmeshloader.h"

#include "Modules/keyvalue/valuetype.h"
#include "Modules/serialize/serializetable_impl.h"
#include "Modules/keyvalue/metatable_impl.h"

namespace Engine {
ENTITYCOMPONENTVIRTUALIMPL_IMPL(Render::OpenGLMesh);

namespace Render {
    OpenGLMesh::OpenGLMesh(Scene::Entity::Entity &e, const ObjectPtr &data)
        : Serialize::SerializableUnit<OpenGLMesh, Scene::Entity::Mesh>(e/*, data*/)
        , mResource(nullptr)
    {
        if (const Engine::ValueType &v = data["mesh"]; v.is<std::string>()) {
            setName(v.as<std::string>());
        }
    }

    OpenGLMeshData *OpenGLMesh::data() const
    {
        return mData.get();
    }

    std::string OpenGLMesh::getName() const
    {
        return mResource ? mResource->name() : "";
    }

    void OpenGLMesh::setName(const std::string &name)
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

    void OpenGLMesh::setManual(std::shared_ptr<OpenGLMeshData> data)
    {
        mData = std::move(data);
        mResource = nullptr;
    }
}
}

METATABLE_BEGIN(Engine::Render::OpenGLMesh)
PROPERTY(Name, getName, setName)
METATABLE_END(Engine::Render::OpenGLMesh)

SERIALIZETABLE_BEGIN(Engine::Render::OpenGLMesh)
SERIALIZETABLE_END(Engine::Render::OpenGLMesh)