#pragma once

#include "refscope.h"
#include "scopeimpl.h"
#include "Serialize\Container\list.h"

namespace Engine {
namespace Scripting {

class MADGINE_EXPORT Vector : public ScopeImpl<Vector, RefScope>
{
public:
	Vector(RefScopeTopLevelSerializableUnit *topLevel);

    bool contains(const ValueType &v);
    ValueType at(int i);
    void setAt(int i, const ValueType &v);
	void resize(size_t size);
    size_t size() const;
    int index(const ValueType &v);


	Serialize::SerializableList<ValueType>::const_iterator begin() const;
	Serialize::SerializableList<ValueType>::const_iterator end() const;

private:
    Serialize::SerializableList<ValueType> mItems;

};

} // namespace Scripting
} // namespace Core

