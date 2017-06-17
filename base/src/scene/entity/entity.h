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
    typedef std::function<std::unique_ptr<EntityComponentBase>(Entity &, const Scripting::ArgumentList &)> ComponentBuilder;

public:
	Entity(const Entity&);
	Entity(Entity &&);

    Entity(const std::string &behaviour, const std::string &name);
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
	std::string getName() const;
	virtual std::string getObjectName() const = 0;

	virtual void setObjectVisible(bool b) = 0;

	virtual void setPosition(const std::array<float, 3> &v) = 0;
	virtual void setScale(const std::array<float, 3> &scale) = 0;
	virtual void setOrientation(const std::array<float, 4> &orientation) = 0;
	virtual void translate(const std::array<float, 3> &v) = 0;
	virtual void rotate(const std::array<float, 4> &q) = 0;

    void onLoad();

	template <class T, class... _Ty>
	T *addComponent_t(_Ty&&... args) {
		if (!hasComponent<T>())
			addComponentImpl(createComponent_t<T>(std::forward<_Ty>(args)...));
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

	void addComponent(const Scripting::ArgumentList &args, const std::string &name);
	void removeComponent(const std::string &name);

	static bool existsComponent(const std::string &name);

	static std::set<std::string> registeredComponentNames();

	virtual void writeState(Serialize::SerializeOutStream &of) const override;
	virtual void readState(Serialize::SerializeInStream &ifs) override;
	virtual void writeCreationData(Serialize::SerializeOutStream &of) const override;

protected:
	
	virtual size_t getSize() const override;

private:

	template <class T, class... _Ty>
	std::unique_ptr<EntityComponentBase> createComponent_t(_Ty&&... args) {
		return std::make_unique<T>(*this, std::forward<_Ty>(args)...);
	}	

	std::tuple<std::unique_ptr<EntityComponentBase>> createComponent(const std::string &name, const Engine::Scripting::ArgumentList &args);
	EntityComponentBase *addComponentImpl(std::unique_ptr<EntityComponentBase> &&component);

	template <class T, class... _Ty>
	class ComponentRegistrator {
	public:
		ComponentRegistrator() {
			const std::string name = T::componentName();
			assert(sRegisteredComponentsByName().find(name) == sRegisteredComponentsByName().end());
			sRegisteredComponentsByName()[name] = [](Entity &e, const Scripting::ArgumentList &args) {
				std::tuple<_Ty...> argsTuple;				
				if (!TupleUnpacker<>::call(&args, &Scripting::ArgumentList::parse<_Ty...>, argsTuple))
					throw 0;
				return TupleUnpacker<Entity &>::call(std::make_unique<T, Entity&, _Ty...>, e, std::move(argsTuple));
			};
		}
		~ComponentRegistrator() {
			const std::string name = T::componentName();
			assert(sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end());
			sRegisteredComponentsByName().erase(name);
		}
	};

	template <class T, class Base, class... _Ty>
	friend class EntityComponent;


	const std::string mName;

	Serialize::ObservableSet<std::unique_ptr<EntityComponentBase>, Serialize::ContainerPolicy::masterOnly, Serialize::ParentCreator<decltype(&Entity::createComponent), &Entity::createComponent>> mComponents;

    static std::map<std::string, ComponentBuilder> &sRegisteredComponentsByName(){
        static std::map<std::string, ComponentBuilder> dummy;
        return dummy;
    }

};

}
}
}
