#pragma once


#include "ObjectState.h"

namespace Engine {


		class MADGINE_BASE_EXPORT MadgineObject {
		protected:
			MadgineObject();
			virtual ~MadgineObject();

		public:
			const char *getName();

			virtual bool init();
			virtual void finalize();

			ObjectState getState();

		private:
			ObjectState mState;

		};

}