#pragma once

#include "objectstate.h"

namespace Engine {

		class MODULES_EXPORT MadgineObject
		{
		public:
			bool callInit();
			bool callInit(int &count);
			void callFinalize();
			void callFinalize(int order);

			virtual const MadgineObject *parent() const = 0;
			//virtual App::Application &app(bool = true) = 0;

			bool isInitialized() const;

		protected:
			MadgineObject();
			virtual ~MadgineObject();

			virtual bool init() = 0;
			virtual void finalize() = 0;
			

			ObjectState getState() const;

			void checkInitState() const;
			void checkDependency() const;
			void markInitialized();

		private:
			ObjectState mState;
			std::string mName;
			int mOrder;
		};

}
