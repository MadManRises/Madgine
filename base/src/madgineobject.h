#pragma once

#include "scripting/types/scopebase.h"

#include "objectstate.h"

namespace Engine {


		class MADGINE_BASE_EXPORT MadgineObject : public Scripting::ScopeBase {
		protected:
			MadgineObject();
			virtual ~MadgineObject();

		public:

			virtual bool init() override;
			virtual void finalize() override;

			ObjectState getState();

		private:
			ObjectState mState;
			std::string mName;

		};

}