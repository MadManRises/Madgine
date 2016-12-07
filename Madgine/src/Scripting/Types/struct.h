#pragma once

#include "refscope.h"
#include "scopeimpl.h"

namespace Engine {
namespace Scripting {



class Struct : public RefScope {
public:
	using RefScope::RefScope;

	virtual std::string getIdentifier() override;

};

} // namespace Scripting
}

