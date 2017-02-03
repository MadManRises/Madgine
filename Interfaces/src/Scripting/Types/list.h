#pragma once

#include "refscope.h"
#include "scopeimpl.h"

namespace Engine {
namespace Scripting {

class INTERFACES_EXPORT List : public ScopeImpl<List, RefScope>
{
public:
	using ScopeImpl::ScopeImpl;

    bool contains(const ValueType &v);
    const ValueType &add(const ValueType &v);
    bool remove(const ValueType &v);
    ValueType at(int i);
    void setAt(int i, const ValueType &v);
	bool empty();
    size_t size();
    int index(const ValueType &v);

    const std::list<ValueType> &data() const;

	std::list<ValueType>::const_iterator begin() const;
	std::list<ValueType>::const_iterator end() const;

    virtual void readState(Serialize::SerializeInStream &ifs) override;
    virtual void writeState(Serialize::SerializeOutStream &of) const override;

private:
    std::list<ValueType> mItems;

};

} // namespace Scripting

template <>
struct INTERFACES_EXPORT Caster<Scripting::List*> {
	static Scripting::List* cast(const ValueType &v);
};

} // namespace Core

