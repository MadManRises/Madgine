#pragma once

#include "Madgine/serialize/serializableunit.h"
#include "Madgine/serialize/container/observed.h"
#include "Madgine/serialize/container/action.h"

#ifdef _WIN32
#include <Windows.h>
#undef min
#undef NO_ERROR
#elif __linux__
using HANDLE=void*;
#endif

namespace Maditor {
	namespace Model {

		class MADITOR_MODEL_EXPORT StatsModel : public QObject, public Engine::Serialize::SerializableUnit<StatsModel> {
			Q_OBJECT
		public:
			StatsModel();

			void reset();

			virtual void timerEvent(QTimerEvent *evt) override;

			void setProcess(HANDLE handle);

			/*void trackAllocations();
			void logTrackedAllocations();
			*/
		protected:
			void dummy();			

		signals:
			void averageFPSChanged(float fps);
			void memoryUsageChanged(size_t mem);
			void ogreMemoryUsageChanged(size_t mem);

		private:
			/*Engine::Serialize::Observed<size_t> mOgreMemoryUsage;
			
			Engine::Serialize::Action<decltype(&StatsModel::dummy), &StatsModel::dummy, Engine::Serialize::ActionPolicy::request> startTrack, stopTrack;			
			*/
			Engine::Serialize::Observed<float> mAverageFPS;		
			

			int mTimerId;
			HANDLE mHandle;
			size_t mCurrentUsage;

		};
	}
}
