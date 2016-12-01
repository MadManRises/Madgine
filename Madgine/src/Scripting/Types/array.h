#pragma once

#include "refscope.h"
#include "scopeimpl.h"

namespace Engine {
namespace Scripting {

class MADGINE_EXPORT Array : public ScopeImpl<Array, RefScope>
{
public:
    Array(size_t size);

    bool contains(const ValueType &v);
    ValueType at(int i);
    void setAt(int i, const ValueType &v);
    size_t size() const;
    int index(const ValueType &v);

    const ValueType *data() const;

	const ValueType *begin() const;
	const ValueType *end() const;

    //virtual void collectNamedValues(std::map<std::string, ValueType*> &values) override;

	virtual void storeCreationData(Serialize::SerializeOutStream &of) override;

	static std::string getClassIdentifier();

protected:

    virtual void collectValueRefs(std::list<ValueType *> &values) override;

    virtual void load(Serialize::SerializeInStream &ifs) override;
    virtual void save(Serialize::SerializeOutStream &of) const override;

private:
    ValueType *mItems;
	size_t mSize;

};

} // namespace Scripting
} // namespace Core

