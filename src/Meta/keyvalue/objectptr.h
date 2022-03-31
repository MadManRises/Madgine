#pragma once


namespace Engine {

struct META_EXPORT ObjectPtr {

    ObjectPtr() = default;
    ObjectPtr(Void);
    ObjectPtr(const std::shared_ptr<ObjectInstance> &instance);

    void setValue(std::string_view name, const ValueType &value);
    bool getValue(ValueType &retVal, std::string_view name) const;

    void reset();
    ObjectInstance *get();
    const ObjectInstance *get() const;

    explicit operator bool() const;

    bool operator==(const ObjectPtr &other) const;

private:
    std::shared_ptr<ObjectInstance> mInstance;
};
}

