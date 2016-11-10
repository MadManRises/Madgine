#pragma once

#include "Scripting/Types/scopeimpl.h"

#include "baseentitycomponent.h"

#include "Scripting\Parsing\entitynodeptr.h"

namespace Engine {
namespace Scene {
namespace Entity {




class MADGINE_EXPORT Entity : public Scripting::ScopeImpl<Entity>
{
private:
    typedef BaseEntityComponent *(Entity::*ComponentAdder)();

public:
    static Entity *entityFromMovable(Ogre::MovableObject *o);

	Entity(const Entity&) = delete;
    Entity(Ogre::SceneNode *node,
           const std::string &behaviour, Ogre::Entity *obj);
    ~Entity();

    void init(const Scripting::ArgumentList &args = {});
	void finalize();
	void remove();

	void positionChanged(const Ogre::Vector3 &dist);
	void update(float timeSinceLastFrame);

	Ogre::SceneNode *createDecoratorNode(bool centered, float height = 0.0f);
	void destroyDecoratorNode(Ogre::SceneNode* node);

	Ogre::SceneNode *getNode();
	Ogre::Entity *getObject();
	const Ogre::Vector3 &getPosition() const;
	Ogre::Vector3 getCenter() const;
	Ogre::Vector2 getPosition2D() const;
	Ogre::Vector2 getCenter2D() const;

	std::string getIdentifier() override;
	std::string getName();

	void setObjectVisible(bool b);

	void setPosition(const Ogre::Vector3 &v);
	void rotate(const Ogre::Quaternion &q);

    void onLoad();

	template <class T>
	T *addComponent_t() {
		if (!hasComponent<T>())
			addComponentImpl(T::componentName(), OGRE_MAKE_UNIQUE(T)(*this));
		return getComponent<T>();
	}

	template <class T>
	T *getComponent() {
		auto it = mComponents.find(T::componentName());
		if (it == mComponents.end())
			return 0;
		return static_cast<T*>(it->second.get());
	}

	template <class T>
	bool hasComponent() {
		return hasComponent(T::componentName());
	}

	bool hasComponent(const std::string &name);

	BaseEntityComponent *addComponent(const std::string &name);
	void removeComponent(const std::string &name);

	static bool existsComponent(const std::string &name);

	static std::set<std::string> registeredComponentNames();


protected:

	Scripting::ValueType methodCall(const std::string &name, const Scripting::ArgumentList &args = {}) override;

	template <class T>
	BaseEntityComponent *addComponentBaseImpl() {
		return addComponent_t<T>();
	}

	
	bool hasScriptMethod(const std::string &name) override;

    const Scripting::Parsing::MethodNodePtr &getMethod(const std::string &name) override;

    virtual void save(Scripting::Serialize::SerializeOutStream &of) const override;
    virtual void load(Scripting::Serialize::SerializeInStream &ifs) override;

	virtual void storeCreationData(Scripting::Serialize::SerializeOutStream &of) override;

	

private:

	template <class T>
	static void registerComponent() {
		const char *name = T::componentName();
		assert(sRegisteredComponentsByName().find(name) == sRegisteredComponentsByName().end());
		sRegisteredComponentsByName()[name] = &addComponentBaseImpl<T>;
	}

	template <class T>
	static void unregisterComponent() {
		const char *name = T::componentName();
		assert(sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end());
		sRegisteredComponentsByName().erase(name);
	}


	void addComponentImpl(const std::string &name, Ogre::unique_ptr<BaseEntityComponent> &&component);

	template <class T>
	class ComponentRegistrator {
	public:
		ComponentRegistrator() {
			registerComponent<T>();
		}
		~ComponentRegistrator() {
			unregisterComponent<T>();
		}
	};

	template <class T>
	friend class EntityComponent;


    const Scripting::Parsing::EntityNodePtr mDescription;
    
	Ogre::SceneNode *mNode;
    Ogre::SceneNode *mDecoratorNode;
	Ogre::Entity *mObject;
    
    Ogre::Vector3 mLastPosition;

    std::map<std::string, Ogre::unique_ptr<BaseEntityComponent>> mComponents;

    static std::map<std::string, ComponentAdder> &sRegisteredComponentsByName(){
        static std::map<std::string, ComponentAdder> dummy;
        return dummy;
    }

};

}
}
}
