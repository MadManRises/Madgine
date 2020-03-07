#include "../moduleslib.h"

#include "keyvaluepair.h"

namespace Engine {

ValueType &KeyValuePair_key(KeyValuePair &p)
{
    return p.mKey;
}

ValueTypeRef &KeyValuePair_value(KeyValuePair &p)
{
    return p.mValue;
}

const ValueType &getArgument(const ArgumentList &args, size_t index)
{
    return args.at(index);
}

#define VALUETYPE_SEP
#define VALUETYPE_TYPE(Type, Storage, Name)                                                                            \
    template <>                                                                                                        \
    MODULES_EXPORT void to_ValueType_impl<Type>(ValueType & v, Type && t) { v = std::move(t); }                        \
                                                                                                                       \
    template <>                                                                                                        \
    MODULES_EXPORT void to_ValueType_impl<Type &>(ValueType & v, Type & t) { v = t; }                                  \
                                                                                                                       \
    template <>                                                                                                        \
    MODULES_EXPORT void to_ValueType_impl<const Type>(ValueType & v, const Type &&t) { v = std::move(t); }             \
                                                                                                                       \
    template <>                                                                                                        \
    MODULES_EXPORT void to_ValueType_impl<const Type &>(ValueType & v, const Type &t) { v = t; }                       \
                                                                                                                       \
    template <>                                                                                                        \
    MODULES_EXPORT void to_ValueTypeRef_impl<Type>(ValueTypeRef & v, Type && t) { v = ValueTypeRef { std::move(t) }; } \
                                                                                                                       \
    template <>                                                                                                        \
    MODULES_EXPORT void to_ValueTypeRef_impl<Type&>(ValueTypeRef & v, Type & t) { v = ValueTypeRef { t }; }             \
                                                                                                                       \
    template <>                                                                                                        \
    MODULES_EXPORT ValueType_Return<Type> ValueType_as_impl<Type>(const ValueType &v) { return v.as<Type>(); }

#include "valuetypedef.h"

}