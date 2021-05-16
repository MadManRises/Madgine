#pragma once


namespace Engine {

struct META_EXPORT ObjectPtr {

    ObjectPtr();
    ObjectPtr(const std::shared_ptr<ObjectInstance> &instance);

    void setValue(const std::string_view &name, const ValueType &value);
    bool getValue(ValueType &retVal, const std::string_view &name) const;

    void reset();

    explicit operator bool() const;

    bool operator==(const ObjectPtr &other) const;

private:
    std::shared_ptr<ObjectInstance> mInstance;
};
}

