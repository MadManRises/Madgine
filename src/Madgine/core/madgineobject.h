#pragma once

#include "objectstate.h"

namespace Engine
{
	namespace Core {

		class MADGINE_BASE_EXPORT MadgineObject
		{
		public:
			bool callInit();
			bool callInit(int &count);
			void callFinalize();
			void callFinalize(int order);

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
}
