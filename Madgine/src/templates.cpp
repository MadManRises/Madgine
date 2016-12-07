#include "madginelib.h"

#include "Scripting\Types\scope_cast.h"
#include "valuetype.h"
#include "templates.h"
#include "Scene\Entity\entity.h"
#include "Scripting\Types\list.h"

namespace Engine {

	Scripting::List *Caster<Scripting::List*>::cast(const ValueType & v) {
		Scripting::List *list = Scripting::scope_cast<Scripting::List>(v.asScope());
		if (!list)
			throw 0;
		return list;
	}


	Scene::Entity::Entity *Caster<Scene::Entity::Entity*>::cast(const ValueType & v) {
		Scene::Entity::Entity *entity = Scripting::scope_cast<Scene::Entity::Entity>(v.asScope());
		if (!entity)
			throw 0;
		return entity;
	}

}