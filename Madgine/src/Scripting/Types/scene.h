#pragma once

#include "scopefactoryimpl.h"

namespace Engine {
namespace Scripting {

class Scene : public ScopeFactoryImpl<Scene>
{
public:
    Scene();

	virtual void clear() override;

};

} // namespace Scripting
} // namespace Core

