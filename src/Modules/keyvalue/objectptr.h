#pragma once


namespace Engine {

struct MODULES_EXPORT ObjectPtr {

    ObjectPtr();
    ObjectPtr(const std::shared_ptr<ObjectInstance> &instance);

    void setValue(const std::string &name, const ValueType &value);
    ValueType getValue(const std::string &name) const;

    void clear();

    ObjectFieldAccessor operator[](const std::string &name);
    ObjectFieldAccessor operator[](const char *name);
    ValueType operator[](const std::string &name) const;
    ValueType operator[](const char *name) const;
    operator bool() const;

private:
    std::shared_ptr<ObjectInstance> mInstance;
};
}

