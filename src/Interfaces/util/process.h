#pragma once

#include "../signalslot/signal.h"

namespace Engine
{
	namespace Util
	{
		class INTERFACES_EXPORT Process
		{
		public:
			Process();
			virtual ~Process();

			void step();
			void startSubProcess(size_t size, const std::string& name = "");
			void endSubProcess();

			SignalSlot::SignalStub<const std::string &>& subprocessSignal();
			SignalSlot::SignalStub<float> &ratioSignal();

		private:
			float mRatio;

			std::vector<float> mStepSizes;
			std::vector<size_t> mSubProcessCounts;
			std::vector<size_t> mSubProcessIndices;

			SignalSlot::Signal<const std::string &> mSubProcessStarted;
			SignalSlot::Signal<float> mRatioChanged;
		};
	}
}
