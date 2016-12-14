#pragma once

#include "Serialize\toplevelserializableunit.h"
#include "Serialize\Container\list.h"
#include "array.h"
#include "struct.h"
#include "list.h"

namespace Engine {
	namespace Scripting {

		class RefScopeTopLevelSerializableUnit : public Serialize::TopLevelSerializableUnit {

		public:
			RefScopeTopLevelSerializableUnit(Serialize::TopLevelMadgineObject type);

			Scripting::Vector *createArray();
			Scripting::Struct *createStruct();
			Scripting::List *createList();

			void removeRefScope(Scripting::RefScope *scope);

		protected:
			std::tuple<RefScopeTopLevelSerializableUnit*> createRefScopeImpl();
			

		private:
			Serialize::SerializableList<Scripting::Vector, RefScopeTopLevelSerializableUnit*> mArrays;
			Serialize::SerializableList<Scripting::Struct, RefScopeTopLevelSerializableUnit*> mStructs;
			Serialize::SerializableList<Scripting::List, RefScopeTopLevelSerializableUnit*> mLists;
		};

	}
}