#include "madginelib.h"
#include "array.h"
#include "Scripting/scriptingexception.h"
#include "Serialize/Streams/serializestream.h"

namespace Engine {

	
	namespace Scripting {



API_IMPL(Array, &contains, &at, &setAt, &size, &index);


Array::Array(size_t size) :
	mSize(size),
	mItems(new ValueType[size]())
{

}


bool Array::contains(const ValueType &v)
{
    return std::find(begin(), end(), v) != end();
}

ValueType Array::at(int i)
{
	if (i >= mSize)
		MADGINE_THROW_NO_TRACE(ScriptingException("index out of bounds!"));
	return mItems[i];
}

void Array::setAt(int i, const ValueType & v)
{
	if (i >= mSize)
		MADGINE_THROW_NO_TRACE(ScriptingException("index out of bounds!"));
	mItems[i] = v;
}

size_t Array::size() const
{
	return mSize;
}

int Array::index(const ValueType & v)
{
	auto it = std::find(begin(), end(), v);
	if (it == end()) return -1;
	else return std::distance(begin(), it);
}



void Array::readState(Serialize::SerializeInStream &ifs)
{

    Scope::readState(ifs);

    size_t count;
    ifs >> count;
	if (count != mSize)
		MADGINE_THROW_NO_TRACE(ScriptingException("serialized data array size mismatch!"));

	for (auto v : *this) {
        ifs >> v;
    }

}

void Array::writeState(Serialize::SerializeOutStream &of) const
{
    Scope::writeState(of);

    of << mSize;

    for (const ValueType &v : *this){
        of << v;
    }

}

const ValueType *Array::data() const
{
    return mItems;
}

const ValueType *Array::begin() const
{
	return mItems;
}

const ValueType *Array::end() const
{
	return mItems + mSize;
}

/*void List::collectNamedValues(std::map<std::string, ValueType *> &values)
{
    Scope::collectNamedValues(values);

    unsigned int i = 0;
    for (ValueType &v : *this){
        values[std::to_string(i++)] = &v;
    }
}*/


void Array::writeCreationData(Serialize::SerializeOutStream & of) const
{
	Scope::writeCreationData(of);
	of << mSize;
}



void Array::collectValueRefs(std::list<ValueType *> &values)
{
    Scope::collectValueRefs(values);

	for (size_t i = 0; i < mSize; ++i) {
        values.push_back(mItems + i);
    }
}

} // namespace Scripting


} // namespace Core

