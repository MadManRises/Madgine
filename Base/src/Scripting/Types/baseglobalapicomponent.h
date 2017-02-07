#pragma once

#include "MadgineObject.h"

namespace Engine {
	namespace Scripting {

		class MADGINE_BASE_EXPORT BaseGlobalAPIComponent : public BaseMadgineObject {

		public:
			virtual ~BaseGlobalAPIComponent() = default;

			virtual void clear();
		};

	}
}