#include "madginelib.h"

#include "refscopetoplevelserializableunit.h"


namespace Engine {
	namespace Scripting {



		RefScopeTopLevelSerializableUnit::RefScopeTopLevelSerializableUnit(Serialize::TopLevelMadgineObject type) :
			mArrays(this, &RefScopeTopLevelSerializableUnit::createRefScopeImpl),
			mStructs(this, &RefScopeTopLevelSerializableUnit::createRefScopeImpl),
			mLists(this, &RefScopeTopLevelSerializableUnit::createRefScopeImpl),
			TopLevelSerializableUnit(type)
		{
		}

		Scripting::Vector *RefScopeTopLevelSerializableUnit::createArray()
		{
			return &mArrays.emplaceTuple(createRefScopeImpl());
		}

		Scripting::Struct * RefScopeTopLevelSerializableUnit::createStruct()
		{
			return &mStructs.emplaceTuple(createRefScopeImpl());
		}

		Scripting::List * RefScopeTopLevelSerializableUnit::createList()
		{
			return &mLists.emplaceTuple(createRefScopeImpl());
		}

		void RefScopeTopLevelSerializableUnit::removeRefScope(Scripting::RefScope * scope)
		{
			Scripting::Vector *array = dynamic_cast<Scripting::Vector*>(scope);
			if (array) {
				auto it = std::find_if(mArrays.begin(), mArrays.end(), [=](const Scripting::Vector &a) {return &a == array; });
				if (it == mArrays.end())
					throw 0;
				mArrays.erase(it);
				return;
			}
			Scripting::List *list = dynamic_cast<Scripting::List*>(scope);
			if (list) {
				auto it = std::find_if(mLists.begin(), mLists.end(), [=](const Scripting::List &l) {return &l == list; });
				if (it == mLists.end())
					throw 0;
				mLists.erase(it);
				return;
			}
			Scripting::Struct *str = dynamic_cast<Scripting::Struct*>(scope);
			if (str) {
				auto it = std::find_if(mStructs.begin(), mStructs.end(), [=](const Scripting::Struct &s) {return &s == str; });
				if (it == mStructs.end())
					throw 0;
				mStructs.erase(it);
				return;
			}
			
			throw 0;			
		}

		std::tuple<RefScopeTopLevelSerializableUnit*> RefScopeTopLevelSerializableUnit::createRefScopeImpl()
		{
			return{ this };
		}

	}
}