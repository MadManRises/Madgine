#pragma once

namespace Engine {


struct Accessor {
    void (*mGetter)(ValueType &, const ScopePtr &);
    void (*mSetter)(const ScopePtr &, const ValueType &);
    bool mIsGeneric;
};

}