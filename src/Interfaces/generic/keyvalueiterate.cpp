#include "../interfaceslib.h"
#include "keyvalueiterate.h"

namespace Engine
{
	std::unique_ptr<KeyValueIterator> KeyValueMapList::find(const std::string& key)
	{
		for (const std::unique_ptr<KeyValueRef>& p : *this)
		{
			std::shared_ptr<KeyValueIterator> v = p->find(key);
			if (!v->ended())
				return std::make_unique<KeyValueMapListIterator>(std::vector<std::shared_ptr<KeyValueIterator>>{std::move(v)});
		}
		return std::make_unique<KeyValueMapListIterator>();
	}

	std::optional<ValueType> KeyValueMapList::get(const std::string& key)
	{
		for (const std::unique_ptr<KeyValueRef>& p : *this)
		{
			std::optional<ValueType> v = p->get(key);
			if (v)
				return v;
		}
		return {};
	}

	bool KeyValueMapList::set(const std::string& key, const ValueType& value)
	{
		for (const std::unique_ptr<KeyValueRef>& p : *this)
		{
			bool b = p->set(key, value);
			if (b)
				return b;
		}
		return false;
	}


	bool KeyValueMapList::contains(const std::string& key)
	{
		for (const std::unique_ptr<KeyValueRef>& p : *this)
		{
			if (p->contains(key))
				return true;
		}
		return false;
	}

	std::unique_ptr<KeyValueIterator> KeyValueMapList::iterator()
	{
		return std::make_unique<KeyValueMapListIterator>(*this);
	}

	KeyValueMapListIterator::KeyValueMapListIterator(const KeyValueMapList& list) :
		mIndex(0)
	{
		mIterators.reserve(list.size());
		std::transform(list.begin(), list.end(), back_inserter(mIterators),
		               [](const std::unique_ptr<KeyValueRef>& m)
		               {
			               return m->iterator();
		               });
		validate();
	}

	KeyValueMapListIterator::KeyValueMapListIterator(std::vector<std::shared_ptr<KeyValueIterator>>&& elements) :
	mIndex(0),
	mIterators{std::forward<std::vector<std::shared_ptr<KeyValueIterator>>>(elements)}
	{
		validate();
	}

	std::string KeyValueMapListIterator::key() const
	{
		return mIterators.at(mIndex)->key();
	}

	ValueType KeyValueMapListIterator::value() const
	{
		return mIterators.at(mIndex)->value();
	}

	KeyValueValueFlags KeyValueMapListIterator::flags() const
	{
		return mIterators.at(mIndex)->flags();
	}

	void KeyValueMapListIterator::operator++()
	{
		++*mIterators.at(mIndex);
		validate();
	}

	bool KeyValueMapListIterator::ended() const
	{
		return mIndex == mIterators.size();
	}

	void KeyValueMapListIterator::validate()
	{
		while (!ended() && mIterators.at(mIndex)->ended())
			++mIndex;
	}

	KeyValueMapList KeyValueMapList::merge(KeyValueMapList&& other) &&
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

	size_t KeyValueMapList::size() const
	{
		return mRefs.size();
	}
}
