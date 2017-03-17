#pragma once

#include "MadgineObject.h"

namespace Engine {
	namespace Scripting {

		class MADGINE_BASE_EXPORT GlobalAPIComponentBase : public MadgineObject {

		public:
			virtual ~GlobalAPIComponentBase() = default;

			virtual void clear();
			virtual void update(float timeSinceLastFrame);
		};

	}
}