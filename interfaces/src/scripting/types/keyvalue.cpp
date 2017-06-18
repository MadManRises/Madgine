

#include "interfaceslib.h"
#include "keyvalue.h"

namespace Engine {
	namespace Scripting {
		
		std::pair<bool, ValueType> toValueType(...) {
			return { false,{} };
		}

		int KeyValueMapList::resolve(lua_State * state, const std::string & key)
		{
			for (const std::unique_ptr<KeyValueMapRef> &p : *this) {
				if (int i = p->resolve(state, key))
					return i;
			}
			return 0;
		}

		KeyValueIterator * KeyValueMapList::iterator()
		{
			return new KeyValueMapListIterator(*this);
		}

		KeyValueMapListIterator::KeyValueMapListIterator(const KeyValueMapList & list) :
			mIndex(0)
		{
			mIterators.reserve(list.size());
			std::transform(list.begin(), list.end(), std::back_inserter(mIterators),
				[](const std::unique_ptr<KeyValueMapRef> &m) {return std::unique_ptr<KeyValueIterator>(m->iterator()); });
			validate();
		}

		std::string KeyValueMapListIterator::key()
		{
			return mIterators.at(mIndex)->key();
		}

		std::pair<bool, ValueType> Engine::Scripting::KeyValueMapListIterator::value()
		{
			return mIterators.at(mIndex)->value();
		}

		void KeyValueMapListIterator::operator++()
		{
			++(*mIterators.at(mIndex));
			validate();
		}

		int KeyValueMapListIterator::push(lua_State * state)
		{
			return mIterators.at(mIndex)->push(state);
		}

		bool KeyValueMapListIterator::ended()
		{
			return mIndex == mIterators.size();
		}

		void KeyValueMapListIterator::validate()
		{
			while (!ended() && mIterators.at(mIndex)->ended())
				++mIndex;
		}

		KeyValueMapList &&KeyValueMapList::merge(KeyValueMapList && other) &&
		{
			mMaps.reserve(size() + other.size());
			std::move(other.mMaps.begin(), other.mMaps.end(), std::back_inserter(mMaps));
			return std::forward<KeyValueMapList>(*this);
		}

		std::vector<std::unique_ptr<KeyValueMapRef>>::const_iterator KeyValueMapList::begin() const
		{
			return mMaps.begin();
		}

		std::vector<std::unique_ptr<KeyValueMapRef>>::const_iterator KeyValueMapList::end() const
		{
			return mMaps.end();
		}

		size_t KeyValueMapList::size() const {
			return mMaps.size();
		}

	}
}
