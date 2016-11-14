#pragma once

#include "refscope.h"
#include "scopeimpl.h"

namespace Engine {
namespace Scripting {

class MADGINE_EXPORT List : public ScopeImpl<List, RefScope>
{
public:
    List();

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

    virtual void collectNamedValues(std::map<std::string, ValueType*> &values) override;

protected:
    std::string getIdentifier();

    virtual void collectValueRefs(std::list<ValueType *> &values) override;

    virtual void load(Serialize::SerializeInStream &ifs) override;
    virtual void save(Serialize::SerializeOutStream &of) const override;

private:
    std::list<ValueType> mItems;

};

} // namespace Scripting
} // namespace Core

