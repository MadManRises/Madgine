#pragma once

#include "scope.h"
#include "scopefactoryimpl.h"

namespace Engine {
namespace Scripting {

class Level : public ScopeFactoryImpl<Level>
{
public:
    Level();

	virtual void clear() override;

protected:
    std::string getIdentifier();

};

} // namespace Scripting
} // namespace Core

