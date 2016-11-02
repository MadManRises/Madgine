#pragma once

#include "Util\MadgineObject.h"

namespace Engine {
	namespace Scripting {

		class MADGINE_EXPORT BaseGlobalAPIComponent : public Util::BaseMadgineObject {

		public:
			virtual ~BaseGlobalAPIComponent() = default;

			virtual void clear();
		};

	}
}