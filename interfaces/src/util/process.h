#pragma once

#include "signalslot/signal.h"

namespace Engine {
	namespace Util {

		class INTERFACES_EXPORT Process
		{
		public:
			Process();
			~Process();

			void step();
			void startSubProcess(size_t size, const std::string &name = "");
			void endSubProcess();

			template <class T>
			void connectSubProcessStarted(T &&slot) {
				mSubProcessStarted.connect(std::forward<T>(slot), SignalSlot::QueuedConnectionType{});
			}

			template <class T>
			void connectRatioChanged(T &&slot) {
				mRatioChanged.connect(std::forward<T>(slot), SignalSlot::QueuedConnectionType{});
			}

		private:
			float mRatio;

			std::list<float> mStepSizes;
			std::list<size_t> mSubProcessCounts;
			std::list<size_t> mSubProcessIndices;

			SignalSlot::Signal<const std::string &> mSubProcessStarted;
			SignalSlot::Signal<float> mRatioChanged;
		};

	}
}


