#include "../moduleslib.h"

#include "nativeobject.h"
#include "objectinstance.h"
#include "valuetype.h"

namespace Engine {

struct NativeObjectInstance : ObjectInstance {
    NativeObjectInstance(std::map<std::string, ValueType> data)
        : mData(std::move(data))
    {
    }

    virtual ValueType getValue(const std::string &name) const override
    {
        auto it = mData.find(name);
        return it == mData.end() ? ValueType {} : it->second;
    }

    virtual void setValue(const std::string &name, const ValueType &value) override
    {
        mData[name] = value;
    }

private:
    std::map<std::string, ValueType> mData;
};

NativeObject::NativeObject(std::map<std::string, ValueType> data)
    : ObjectPtr(std::make_shared<NativeObjectInstance>(std::move(data)))
{
}

}