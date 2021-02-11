#pragma once

namespace Engine {


struct Accessor {
    void (*mGetter)(ValueType &, const TypedScopePtr &);
    void (*mSetter)(const TypedScopePtr &, const ValueType &);
    bool mIsGeneric;
};

}