#pragma once

namespace Engine {

struct ObjectInstance {
    virtual ~ObjectInstance() = default;

    virtual bool getValue(ValueType &retVal, std::string_view name) const = 0;
    virtual void setValue(std::string_view name, const ValueType &value) = 0;

    virtual void call(ValueType &retVal, const ArgumentList &args)
    {
        throw 0;
    }
};

}