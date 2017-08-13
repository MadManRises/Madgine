#pragma once

#include "entitycomponentbase.h"
#include "entity.h"
#include "scripting/types/scope.h"

namespace Engine{
namespace Scene {
namespace Entity {

	template <class T, class Base = EntityComponentBase>
	class EntityComponent : public Scripting::Scope<T, Base> {
	public:
		EntityComponent(Entity &entity, const Scripting::LuaTable &table) :
			Scope(entity, table) {

		}


    virtual std::string getName() const override{
        return sComponentName;
    }

	virtual std::string key() const override {
		return sComponentName;
	}

	static const char * const componentName() {
		return sComponentName;
	}
	

private:
	virtual void __reg() {
		(void)_reg;
	}

	virtual void writeCreationData(Serialize::SerializeOutStream &out) const override {
		out << sComponentName;
	}

	virtual size_t getSize() const override final {
		return sizeof(T);
	}
    
	static const char * const sComponentName;
    static const Entity::ComponentRegistrator<T> _reg;

};

template <class T, class Base>
const Entity::ComponentRegistrator<T> EntityComponent<T, Base>::_reg;

}
}
}


