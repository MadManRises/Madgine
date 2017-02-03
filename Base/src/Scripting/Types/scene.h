#pragma once

#include "Scripting/Types/scope.h"

namespace Engine {
namespace Scripting {

class Scene : public Scope
{
public:

	virtual std::string getIdentifier() override;

};

} // namespace Scripting
} // namespace Core

