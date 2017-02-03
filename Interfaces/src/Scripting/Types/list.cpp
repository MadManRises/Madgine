#include "interfaceslib.h"
#include "list.h"
#include "Scripting/scriptingexception.h"
#include "Serialize/Streams/serializestream.h"
#include "scope_cast.h"

namespace Engine {

namespace Scripting {



API_IMPL(List, &contains, &add, &at, &setAt, &size, &remove, &index);



bool List::contains(const ValueType &v)
{
    return std::find(mItems.begin(), mItems.end(), v) != mItems.end();
}

const ValueType &List::add(const ValueType & v)
{
	mItems.push_back(v);
	return mItems.back();
}

bool List::remove(const ValueType & v)
{
	auto it = std::find(mItems.begin(), mItems.end(), v);
	if (it == mItems.end())
		return false;
	mItems.erase(it);
	return true;
}

ValueType List::at(int i)
{
	auto it = mItems.begin();
	std::advance(it, i);
	return *it;
}

void List::setAt(int i, const ValueType & v)
{
	auto it = mItems.begin();
	std::advance(it, i);
	*it = v;
}

bool List::empty()
{
	return mItems.empty();
}

size_t List::size()
{
	return mItems.size();
}

int List::index(const ValueType & v)
{
	auto it = std::find(mItems.begin(), mItems.end(), v);
	if (it == mItems.end()) return -1;
	else return std::distance(mItems.begin(), it);
}



void List::readState(Serialize::SerializeInStream &ifs)
{

    Scope::readState(ifs);

    size_t count;
    ifs >> count;

    mItems.resize(count);

    for (ValueType &v : mItems){
        ifs >> v;
    }

}

void List::writeState(Serialize::SerializeOutStream &of) const
{
    Scope::writeState(of);

    of << mItems.size();

    for (const ValueType &v : mItems){
        of << v;
    }

}

const std::list<ValueType> &List::data() const
{
    return mItems;
}

std::list<ValueType>::const_iterator List::begin() const
{
	return mItems.begin();
}

std::list<ValueType>::const_iterator List::end() const
{
	return mItems.end();
}


} // namespace Scripting



Scripting::List *Caster<Scripting::List*>::cast(const ValueType & v) {
	Scripting::List *list = Scripting::scope_cast<Scripting::List>(v.asScope());
	if (!list)
		throw 0;
	return list;
}


} // namespace Core

