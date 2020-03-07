#pragma once

namespace Engine {


struct Accessor {
    void (*mGetter)(ValueType &, TypedScopePtr);
    void (*mSetter)(TypedScopePtr, const ValueType &);
};

}