#pragma once

#include "scopefactoryimpl.h"

namespace Engine {
namespace Scripting {

class Scene : public ScopeFactoryImpl<Scene>
{
public:
    Scene();

	virtual void clear() override;

protected:
    std::string getIdentifier();

};

} // namespace Scripting
} // namespace Core

