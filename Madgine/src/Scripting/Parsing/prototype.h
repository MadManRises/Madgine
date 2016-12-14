#pragma once

#include "Scripting/Types/scene.h"

namespace Engine {
	namespace Scripting {

		class Prototype : public Scope {
		public:
			using Scope::Scope;

			virtual std::string getIdentifier() override;

		};

	} // namespace Scripting
}

