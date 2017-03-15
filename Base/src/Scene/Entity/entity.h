#pragma once

#include "Scripting/Types/scope.h"

#include "EntityComponentBase.h"

#include "Serialize\Container\set.h"



namespace Engine {
namespace Scene {
namespace Entity {




class MADGINE_BASE_EXPORT Entity : public Scripting::Scope<Entity>
{
private:
    typedef std::unique_ptr<EntityComponentBase> (Entity::*ComponentBuilder)();

public:

	Entity(const Entity&);
	Entity(Entity &&);

    Entity(const Scripting::Parsing::EntityNode *behaviour);
    ~Entity();

    void init(const Scripting::ArgumentList &args = {});
	void remove();

	virtual std::array<float, 3> getPosition() const = 0;
	virtual std::array<float, 3> getCenter() const = 0;
	std::array<float, 2> getPosition2D() const;
	std::array<float, 2> getCenter2D() const;
	virtual std::array<float, 4> getOrientation() const = 0;
	virtual std::array<float, 3> getScale() const = 0;

	virtual std::string getIdentifier() override;
	virtual std::string getName() const = 0;
	virtual std::string getObjectName() const = 0;

	virtual void setObjectVisible(bool b) = 0;

	virtual void setPosition(const std::array<float, 3> &v) = 0;
	virtual void setScale(const std::array<float, 3> &scale) = 0;
	virtual void setOrientation(const std::array<float, 4> &orientation) = 0;
	virtual void translate(const std::array<float, 3> &v) = 0;
	virtual void rotate(const std::array<float, 4> &q) = 0;

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
	std::unique_ptr<EntityComponentBase> createComponent_t() {
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


	std::tuple<std::unique_ptr<EntityComponentBase>> createComponent(const std::string &name);
	EntityComponentBase *addComponentImpl(std::unique_ptr<EntityComponentBase> &&component);

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

	template <class T, class Base>
	friend class EntityComponent;


    const Scripting::Parsing::EntityNode *mDescription;    

	Serialize::ObservableSet<std::unique_ptr<EntityComponentBase>, Serialize::ContainerPolicy::masterOnly, Serialize::CustomCreator<decltype(&Entity::createComponent), &Entity::createComponent>> mComponents;

    static std::map<std::string, ComponentBuilder> &sRegisteredComponentsByName(){
        static std::map<std::string, ComponentBuilder> dummy;
        return dummy;
    }

};

}
}
}
