#pragma once

#include "MadgineObject.h"

namespace Engine {
	namespace Scripting {

		class MADGINE_BASE_EXPORT GlobalAPIComponentBase : public MadgineObjectBase {

		public:
			virtual ~GlobalAPIComponentBase() = default;

			virtual void clear();
			virtual void update(float timeSinceLastFrame);
		};

	}
}