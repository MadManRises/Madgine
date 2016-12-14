#include "madginelib.h"
#include "array.h"
#include "Scripting/scriptingexception.h"
#include "Serialize/Streams/serializestream.h"

namespace Engine {

	
	namespace Scripting {



API_IMPL(Vector, &contains, &at, &setAt, &size, &index);

Vector::Vector(RefScopeTopLevelSerializableUnit * topLevel) :
	mItems(this),
	ScopeImpl(topLevel)
{
}

bool Vector::contains(const ValueType &v)
{
    return std::find(mItems.begin(), mItems.end(), v) != mItems.end();
}

ValueType Vector::at(int i)
{
	return mItems.at(i);
}

void Vector::setAt(int i, const ValueType & v)
{
	mItems.at(i) = v;
}

void Vector::resize(size_t size)
{
	//mItems.resize(size);
}

size_t Vector::size() const
{
	return mItems.size();
}

int Vector::index(const ValueType & v)
{
	auto it = std::find(mItems.begin(), mItems.end(), v);
	if (it == mItems.end()) return -1;
	else return std::distance(mItems.begin(), it);
}



/*const ValueType *Vector::data() const
{
    return mItems.data();
}*/

Serialize::SerializableList<ValueType>::const_iterator Vector::begin() const
{
	return mItems.begin();
}

Serialize::SerializableList<ValueType>::const_iterator Vector::end() const
{
	return mItems.end();
}




} // namespace Scripting


} // namespace Core

