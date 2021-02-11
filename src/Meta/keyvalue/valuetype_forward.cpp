#include "../metalib.h"

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
#define VALUETYPE_IMPL(Type)                                                                                                                       \
    template <>                                                                                                                                    \
    META_EXPORT void to_ValueType_impl<std::decay_t<Type>>(ValueType & v, std::decay_t<Type> && t) { v = std::move(t); }                        \
                                                                                                                                                   \
    template <>                                                                                                                                    \
    META_EXPORT void to_ValueType_impl<std::decay_t<Type> &>(ValueType & v, std::decay_t<Type> & t) { v = t; }                                  \
                                                                                                                                                   \
    template <>                                                                                                                                    \
    META_EXPORT void to_ValueType_impl<const std::decay_t<Type>>(ValueType & v, const std::decay_t<Type> &&t) { v = std::move(t); }             \
                                                                                                                                                   \
    template <>                                                                                                                                    \
    META_EXPORT void to_ValueType_impl<const std::decay_t<Type> &>(ValueType & v, const std::decay_t<Type> &t) { v = t; }                       \
                                                                                                                                                   \
    template <>                                                                                                                                    \
    META_EXPORT void to_ValueTypeRef_impl<std::decay_t<Type>>(ValueTypeRef & v, std::decay_t<Type> && t) { v = ValueTypeRef { std::move(t) }; } \
                                                                                                                                                   \
    template <>                                                                                                                                    \
    META_EXPORT void to_ValueTypeRef_impl<std::decay_t<Type> &>(ValueTypeRef & v, std::decay_t<Type> & t) { v = ValueTypeRef { t }; }           \
                                                                                                                                                   \
    template <>                                                                                                                                    \
    META_EXPORT ValueType_Return<std::decay_t<Type>> ValueType_as_impl<std::decay_t<Type>>(const ValueType &v) { return v.as<std::decay_t<Type>>(); }

#define VALUETYPE_TYPE(Name, Storage, ...) FOR_EACH(VALUETYPE_IMPL, VALUETYPE_SEP, __VA_ARGS__)

#include "valuetypedef.h"
#undef VALUETYPE_IMPL

template <>
META_EXPORT void to_ValueType_impl<ValueType>(ValueType &v, ValueType &&t)
{
    v = std::move(t);
}

template <>
META_EXPORT void to_ValueType_impl<ValueType &>(ValueType &v, ValueType &t)
{
    v = t;
}

}