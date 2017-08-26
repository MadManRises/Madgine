#pragma once

#include "scripting/types/scope.h"

#include "entitycomponentbase.h"

#include "serialize/container/set.h"



namespace Engine {
namespace Scene {
namespace Entity {




class MADGINE_BASE_EXPORT Entity : public Serialize::SerializableUnitBase, public Scripting::Scope<Entity>
{
private:
    typedef std::function<std::unique_ptr<EntityComponentBase>(Entity &, const Scripting::LuaTable &)> ComponentBuilder;

public:
	Entity(const Entity&);
	Entity(Entity &&);

    Entity(SceneManagerBase *sceneMgr, const std::string &name);
    ~Entity();

    
	void remove();

	virtual Vector3 getPosition() const = 0;
	virtual Vector3 getCenter() const = 0;
	std::array<float, 2> getPosition2D() const;
	std::array<float, 2> getCenter2D() const;
	virtual std::array<float, 4> getOrientation() const = 0;
	virtual Vector3 getScale() const = 0;

	virtual std::string getObjectName() const = 0;
	virtual const char *key() const override;

	virtual void setObjectVisible(bool b) = 0;

	virtual void setPosition(const Vector3 &v) = 0;
	virtual void setScale(const Vector3 &scale) = 0;
	virtual void setOrientation(const std::array<float, 4> &orientation) = 0;
	virtual void translate(const Vector3 &v) = 0;
	virtual void rotate(const std::array<float, 4> &q) = 0;

    void onLoad();

	template <class T>
	T *addComponent_t(Entity &e, const Scripting::LuaTable &table) {
		if (!hasComponent<T>())
			addComponentImpl(createComponent_t<T>(*this, e, table));
		return getComponent<T>();
	}

	template <class T>
	T *getComponent() {
		return static_cast<T*>(getComponent(T::componentName()));
	}

	EntityComponentBase *getComponent(const std::string &name);

	template <class T>
	bool hasComponent() {
		return hasComponent(T::componentName());
	}

	bool hasComponent(const std::string &name);

	void addComponent(const std::string &name, const Scripting::LuaTable &table = {});
	void removeComponent(const std::string &name);

	static bool existsComponent(const std::string &name);

	static std::set<std::string> registeredComponentNames();

	virtual void writeState(Serialize::SerializeOutStream &of) const override;
	virtual void readState(Serialize::SerializeInStream &ifs) override;
	virtual void writeCreationData(Serialize::SerializeOutStream &of) const override;

protected:
	
	virtual size_t getSize() const override;
	virtual bool init(const std::string &behaviour);

	virtual KeyValueMapList maps() override;

private:

	template <class T>
	static std::unique_ptr<EntityComponentBase> createComponent_t(Entity &e, const Scripting::LuaTable &table) {
		std::unique_ptr<EntityComponentBase> c = std::make_unique<T>(e, table);
		return c;
	}	

	std::tuple<std::unique_ptr<EntityComponentBase>> createComponent(const std::string &name, const Scripting::LuaTable &table = {});
	std::tuple<std::unique_ptr<EntityComponentBase>> createComponentSimple(const std::string &name) { return createComponent(name); }
	EntityComponentBase *addComponentImpl(std::unique_ptr<EntityComponentBase> &&component);

	template <class T>
	class ComponentRegistrator {
	public:
		ComponentRegistrator() {
			const std::string name = T::componentName();
			assert(sRegisteredComponentsByName().find(name) == sRegisteredComponentsByName().end());
			sRegisteredComponentsByName()[name] = &createComponent_t<T>;			
		}
		~ComponentRegistrator() {
			const std::string name = T::componentName();
			assert(sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end());
			sRegisteredComponentsByName().erase(name);
		}
	};

	static std::map<std::string, ComponentBuilder> &sRegisteredComponentsByName() {
		static std::map<std::string, ComponentBuilder> dummy;
		return dummy;
	}

	template <class T, class Base>
	friend class EntityComponent;


	const std::string mName;

	Serialize::ObservableSet<std::unique_ptr<EntityComponentBase>, Serialize::ContainerPolicy::masterOnly, Serialize::ParentCreator<decltype(&Entity::createComponentSimple), &Entity::createComponentSimple>> mComponents;

	SceneManagerBase *mSceneManager;


};

}
}
}
