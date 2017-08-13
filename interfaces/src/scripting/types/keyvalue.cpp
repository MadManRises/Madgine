

#include "interfaceslib.h"
#include "keyvalue.h"
#include "mapper.h"

namespace Engine {
	namespace Scripting {

		Engine::ValueType toValueType(ScopeBase *ref, const Mapper &mapper) {
			return mapper.mToValueType ? mapper.mToValueType(ref) : ValueType{};
		}

		std::pair<bool, ValueType> KeyValueMapList::get(const std::string & key)
		{
			for (const std::unique_ptr<KeyValueRef> &p : *this) {
				std::pair<bool, ValueType> v = p->get(key);
				if (v.first)
					return v;
			}
			return { false, ValueType{} };
		}

		bool KeyValueMapList::contains(const std::string & key)
		{
			for (const std::unique_ptr<KeyValueRef> &p : *this) {
				if (p->contains(key))
					return true;
			}
			return false;
		}

		std::unique_ptr<KeyValueIterator> KeyValueMapList::iterator()
		{
			return std::make_unique<KeyValueMapListIterator>(*this);
		}

		KeyValueMapListIterator::KeyValueMapListIterator(const KeyValueMapList & list) :
			mIndex(0)
		{
			mIterators.reserve(list.size());
			std::transform(list.begin(), list.end(), std::back_inserter(mIterators),
				[](const std::unique_ptr<KeyValueRef> &m) {
				return m->iterator(); 
			});
			validate();
		}

		std::string KeyValueMapListIterator::key()
		{
			return mIterators.at(mIndex)->key();
		}

		ValueType Engine::Scripting::KeyValueMapListIterator::value()
		{
			return mIterators.at(mIndex)->value();
		}

		void KeyValueMapListIterator::operator++()
		{
			++(*mIterators.at(mIndex));
			validate();
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

		KeyValueMapList KeyValueMapList::merge(KeyValueMapList && other) &&
		{
			mRefs.reserve(size() + other.size());
			std::move(other.mRefs.begin(), other.mRefs.end(), std::back_inserter(mRefs));
			return std::forward<KeyValueMapList>(*this);
		}

		std::vector<std::unique_ptr<KeyValueRef>>::const_iterator KeyValueMapList::begin() const
		{
			return mRefs.begin();
		}

		std::vector<std::unique_ptr<KeyValueRef>>::const_iterator KeyValueMapList::end() const
		{
			return mRefs.end();
		}

		size_t KeyValueMapList::size() const {
			return mRefs.size();
		}

	}
}
