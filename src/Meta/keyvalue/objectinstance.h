#pragma once

namespace Engine {

struct ObjectInstance {
    virtual ~ObjectInstance() = default;

	virtual bool getValue(ValueType &retVal, const std::string_view &name) const = 0;
    virtual void setValue(const std::string_view &name, const ValueType &value) = 0;
};

}