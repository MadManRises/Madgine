#pragma once

#include "scopeimpl.h"

namespace Engine {
namespace Scripting {

class Scene : public Scope
{
public:
    Scene(Serialize::TopLevelSerializableUnit *topLevel);

	virtual std::string getIdentifier() override;

};

} // namespace Scripting
} // namespace Core

