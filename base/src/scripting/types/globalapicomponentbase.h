#pragma once

#include "madgineobject.h"
#include "scripting/types/scopebase.h"

namespace Engine
{
	namespace Scripting
	{
		class MADGINE_BASE_EXPORT GlobalAPIComponentBase : public MadgineObject, public ScopeBase
		{
		public:
			bool init() override;
			void finalize() override;

			virtual void clear();
			virtual void update();

			virtual const char* key() const = 0;
		};
	}
}
