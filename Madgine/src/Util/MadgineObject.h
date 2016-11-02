#pragma once

#include "UtilMethods.h"

namespace Engine {
	namespace Util {

		enum class ObjectState {
			CONSTRUCTED,
			INITIALIZED,
			IN_SCENE,
			IN_SCENE_ABOUT_TO_CLEAR
		};

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