#include "libinclude.h"
#include "list.h"
#include "Scripting/scriptexception.h"
#include "Scripting/Datatypes/Serialize/serializestream.h"

namespace Engine {
namespace Scripting {

List::Factory List::sFactory;

API_IMPL(List, &contains, &add, &at, &setAt, &size, &remove, &index);


List::List()
{

}


bool List::contains(const ValueType &v)
{
    return std::find(mItems.begin(), mItems.end(), v) != mItems.end();
}

const ValueType &List::add(const ValueType & v)
{
	mItems.push_back(v);
	return mItems.back();
}

void List::remove(const ValueType & v)
{
	mItems.remove(v);
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



void List::load(Serialize::SerializeInStream &ifs)
{

    Scope::load(ifs);

    size_t count;
    ifs >> count;

    mItems.resize(count);

    for (ValueType &v : mItems){
        ifs >> v;
    }

}

void List::save(Serialize::SerializeOutStream &of) const
{
    Scope::save(of);

    of << mItems.size();

    for (const ValueType &v : mItems){
        of << v;
    }

}

const std::list<ValueType> &List::data() const
{
    return mItems;
}

void List::collectNamedValues(std::map<std::string, ValueType *> &values)
{
    Scope::collectNamedValues(values);

    unsigned int i = 0;
    for (ValueType &v : mItems){
        values[std::to_string(i++)] = &v;
    }
}


std::string List::getIdentifier()
{
    return "List";
}

void List::collectValueRefs(std::list<ValueType *> &values)
{
    Scope::collectValueRefs(values);

    for (ValueType &v : mItems){
        values.push_back(&v);
    }
}

template <> Scope *List::Factory::create(Serialize::SerializeInStream &in)
{
    return OGRE_NEW List();
}

} // namespace Scripting
} // namespace Core

