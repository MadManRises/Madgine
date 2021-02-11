#pragma once

namespace Engine {

struct ObjectInstance {
    virtual ~ObjectInstance() = default;

	virtual ValueType getValue(const std::string &name) const = 0;
    virtual void setValue(const std::string &name, const ValueType &value) = 0;
};

}