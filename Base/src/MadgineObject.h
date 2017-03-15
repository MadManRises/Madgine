#pragma once


#include "ObjectState.h"

namespace Engine {


		class MADGINE_BASE_EXPORT MadgineObjectBase {
		protected:
			MadgineObjectBase();
			virtual ~MadgineObjectBase();

		public:
			virtual const char *getName() = 0;

			virtual bool init();
			virtual void finalize();

			ObjectState getState();

		private:
			ObjectState mState;

		};

		template <class T>
		class MadgineObject : public MadgineObjectBase {
			virtual const char *getName() override {
				return typeid(T).name();
			}		
		};


}