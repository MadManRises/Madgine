#pragma once

#include "scope.h"

namespace Engine {
	namespace Scripting {

		class INTERFACES_EXPORT GlobalScope : public Singleton<GlobalScope>, public Scope {

		};

	}
}