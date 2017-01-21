#include "madginelib.h"

#include "refscopetoplevelserializableunit.h"


namespace Engine {
	namespace Scripting {



		RefScopeTopLevelSerializableUnit::RefScopeTopLevelSerializableUnit(Serialize::TopLevelMadgineObject type) :
			TopLevelSerializableUnit(type),
			mVectors(this, &RefScopeTopLevelSerializableUnit::createRefScopeImpl),
			mStructs(this, &RefScopeTopLevelSerializableUnit::createRefScopeImpl),
			mLists(this, &RefScopeTopLevelSerializableUnit::createRefScopeImpl)			
		{
		}

		Scripting::Vector *RefScopeTopLevelSerializableUnit::createVector()
		{
			return &*mVectors.emplace_tuple_back(createRefScopeImpl());
		}

		Scripting::Struct * RefScopeTopLevelSerializableUnit::createStruct()
		{
			return &*mStructs.emplace_tuple_back(createRefScopeImpl());
		}

		Scripting::List * RefScopeTopLevelSerializableUnit::createList()
		{
			return &*mLists.emplace_tuple_back(createRefScopeImpl());
		}

		void RefScopeTopLevelSerializableUnit::removeRefScope(Scripting::RefScope * scope)
		{
			Scripting::Vector *vector = dynamic_cast<Scripting::Vector*>(scope);
			if (vector) {
				auto it = std::find_if(mVectors.begin(), mVectors.end(), [=](const Scripting::Vector &v) {return &v == vector; });
				if (it == mVectors.end())
					throw 0;
				mVectors.erase(it);
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
			return std::make_tuple(this);
		}

	}
}