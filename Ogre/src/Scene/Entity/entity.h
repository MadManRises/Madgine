#pragma once

#include "Serialize\ogreSerialize.h"

#include "Scripting/Types/scopeimpl.h"

#include "baseentitycomponent.h"

#include "Serialize\Container\set.h"



namespace Engine {
namespace Scene {
namespace Entity {




class OGREMADGINE_EXPORT Entity : public Scripting::ScopeImpl<Entity>
{
private:
    typedef Ogre::unique_ptr<BaseEntityComponent> (Entity::*ComponentBuilder)();

public:
    static Entity *entityFromMovable(Ogre::MovableObject *o);

	Entity(const Entity&) = delete;
    Entity(Ogre::SceneNode *node,
           const Scripting::Parsing::EntityNode *behaviour, Ogre::Entity *obj);
    ~Entity();

    void init(const Scripting::ArgumentList &args = {});
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
	const Ogre::Quaternion &getOrientation() const;
	const Ogre::Vector3 &getScale() const;

	virtual std::string getIdentifier() override;
	std::string getName() const;

	void setObjectVisible(bool b);

	void setPosition(const Ogre::Vector3 &v);
	void setScale(const Ogre::Vector3 &scale);
	void translate(const Ogre::Vector3 &v);
	void rotate(const Ogre::Quaternion &q);

    void onLoad();

	template <class T>
	T *addComponent_t() {
		if (!hasComponent<T>())
			addComponentImpl(createComponent_t<T>());
		return getComponent<T>();
	}

	template <class T>
	T *getComponent() {
		auto it = mComponents.find(T::componentName());
		if (it == mComponents.end())
			return 0;
		return static_cast<T*>(it->get());
	}

	template <class T>
	bool hasComponent() {
		return hasComponent(T::componentName());
	}

	bool hasComponent(const std::string &name);

	void addComponent(const std::string &name);
	void removeComponent(const std::string &name);

	static bool existsComponent(const std::string &name);

	static std::set<std::string> registeredComponentNames();

	virtual void writeState(Serialize::SerializeOutStream &of) const override;
	virtual void readState(Serialize::SerializeInStream &ifs) override;
	virtual void writeCreationData(Serialize::SerializeOutStream &of) const override;

protected:

	ValueType methodCall(const std::string &name, const Scripting::ArgumentList &args = {}) override;

	template <class T>
	std::unique_ptr<BaseEntityComponent> createComponent_t() {
		return std::make_unique<T>(*this);
	}

	
	bool hasScriptMethod(const std::string &name) override;

    const Scripting::Parsing::MethodNode &getMethod(const std::string &name) override;
	
private:

	template <class T>
	static void registerComponent() {
		const std::string name = T::componentName();
		assert(sRegisteredComponentsByName().find(name) == sRegisteredComponentsByName().end());
		sRegisteredComponentsByName()[name] = &createComponent_t<T>;
	}

	template <class T>
	static void unregisterComponent() {
		const std::string name = T::componentName();
		assert(sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end());
		sRegisteredComponentsByName().erase(name);
	}


	std::tuple<std::unique_ptr<BaseEntityComponent>> createComponent(const std::string &name);
	BaseEntityComponent *addComponentImpl(Ogre::unique_ptr<BaseEntityComponent> &&component);

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


    const Scripting::Parsing::EntityNode *mDescription;
    
	Ogre::SceneNode *mNode;
    Ogre::SceneNode *mDecoratorNode;
	Ogre::Entity *mObject;
    
    Ogre::Vector3 mLastPosition;

	Serialize::ObservableSet<Ogre::unique_ptr<BaseEntityComponent>, Serialize::ContainerPolicy::masterOnly, Ogre::unique_ptr<BaseEntityComponent>> mComponents;
    //std::set<Ogre::unique_ptr<BaseEntityComponent>> mComponents;

    static std::map<std::string, ComponentBuilder> &sRegisteredComponentsByName(){
        static std::map<std::string, ComponentBuilder> dummy;
        return dummy;
    }

};

}
}
}
