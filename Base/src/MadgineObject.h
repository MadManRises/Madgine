#pragma once


#include "ObjectState.h"

namespace Engine {


		class MADGINE_BASE_EXPORT BaseMadgineObject {
		protected:
			BaseMadgineObject();
			virtual ~BaseMadgineObject();

		public:
			virtual const char *getName() = 0;

			virtual bool init();
			virtual void finalize();

			ObjectState getState();

		private:
			ObjectState mState;

		};

		template <class T>
		class MadgineObject : public BaseMadgineObject {
			virtual const char *getName() override {
				return typeid(T).name();
			}		
		};


}