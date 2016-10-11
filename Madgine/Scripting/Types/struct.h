#pragma once

#include "refscope.h"
#include "scopefactoryimpl.h"

namespace Engine {
namespace Scripting {



class Struct : public ScopeFactoryImpl<Struct, RefScope> {
public:
    Struct();

protected:
    std::string getIdentifier();

};

} // namespace Scripting
}

