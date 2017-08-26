#pragma once

#include "madgineobject.h"


namespace Engine {
	namespace Scripting {

		class MADGINE_BASE_EXPORT GlobalAPIComponentBase : public MadgineObject, public Scripting::ScopeBase{

		public:
			virtual bool init() override;
			virtual void finalize() override;

			virtual void clear();
			virtual void update();
		};

	}
}