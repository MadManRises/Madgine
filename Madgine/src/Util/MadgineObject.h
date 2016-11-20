#pragma once

#include "UtilMethods.h"

#include "ObjectState.h"

namespace Engine {
	namespace Util {


		class MADGINE_EXPORT BaseMadgineObject {
		protected:
			BaseMadgineObject();
			virtual ~BaseMadgineObject();

		public:
			virtual const char *getName() = 0;

			virtual void init();
			virtual void finalize();
			virtual void onSceneLoad();
			virtual void beforeSceneClear();
			virtual void onSceneClear();

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
}