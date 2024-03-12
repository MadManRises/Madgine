#pragma once

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/threading/taskfuture.h"

namespace Engine {

struct BehaviorFactoryBase {
    virtual std::vector<std::string_view> names() const = 0;
    virtual Behavior create(std::string_view name, const ParameterTuple &args) const = 0;
    virtual Threading::TaskFuture<ParameterTuple> createParameters(std::string_view name) const = 0;
    virtual std::vector<ValueTypeDesc> parameterTypes(std::string_view name) const = 0;
    virtual std::vector<ValueTypeDesc> resultTypes(std::string_view name) const = 0;
};

struct BehaviorFactoryAnnotation {
    template <typename T>
    BehaviorFactoryAnnotation(type_holder_t<T>)
        : mFactory(&T::sFactory)
    {
    }

    const BehaviorFactoryBase *mFactory;
};

template <typename T>
struct DummyType {
};

struct MADGINE_BEHAVIOR_EXPORT BehaviorHandle {
    Behavior create(const ParameterTuple &args) const;
    Threading::TaskFuture<ParameterTuple> createParameters() const;
    std::vector<ValueTypeDesc> parameterTypes() const;
    std::vector<ValueTypeDesc> resultTypes() const;

    std::string toString() const;
    bool fromString(std::string_view s);

    explicit operator bool() const;

    IndexType<uint32_t> mIndex;
    std::string mName;
};
}

DECLARE_NAMED_UNIQUE_COMPONENT(Engine, BehaviorFactory, BehaviorFactoryBase, BehaviorFactoryAnnotation)

namespace Engine {

template <typename T>
struct BehaviorFactory : BehaviorFactoryComponent<T> {
    static T sFactory;
};

template <typename T>
T BehaviorFactory<T>::sFactory;

}

REGISTER_TYPE(Engine::BehaviorFactoryBase)

#define DECLARE_BEHAVIOR_FACTORY(Factory) \
    REGISTER_TYPE(Factory)

#define DEFINE_BEHAVIOR_FACTORY(Name, Factory) \
    NAMED_UNIQUECOMPONENT(Name, Factory)
