#include "../interfaceslib.h"
#include "process.h"

namespace Engine
{
	namespace Util
	{
		Process::Process() :
			mRatio(0.0f)
		{
		}

		Process::~Process()
		{
			if (mStepSizes.size() > 0)
				LOG("Warning: Unfinished Process!");
		}

		void Process::step()
		{
			++mSubProcessIndices.back();
			if (mSubProcessIndices.back() > mSubProcessCounts.back())
				LOG("Warning: Too many steps!");
			mRatio += mStepSizes.back();
			mRatioChanged.emit(mRatio);
		}

		void Process::startSubProcess(size_t size, const std::string& name)
		{
			if (mSubProcessIndices.size() > 0)
			{
				if (mSubProcessIndices.back() >= mSubProcessCounts.back())
					LOG("Warning: Too many steps!");
				mStepSizes.push_back(mStepSizes.back() / size);
			}
			else
			{
				mStepSizes.push_back(1.0f / size);
			}
			mSubProcessCounts.push_back(size);
			mSubProcessIndices.push_back(0);
			mSubProcessStarted.emit(name);
		}

		void Process::endSubProcess()
		{
			if (mSubProcessIndices.back() != mSubProcessCounts.back())
				LOG("Stepcount not matching!");
			mStepSizes.pop_back();
			if (mSubProcessCounts.back() == 0)
			{
				mRatio += mStepSizes.back();
				mRatioChanged.emit(mRatio);
			}
			mSubProcessCounts.pop_back();
			mSubProcessIndices.pop_back();

			if (mSubProcessIndices.size() > 0)
				++mSubProcessIndices.back();
		}

		SignalSlot::SignalStub<const std::string&>& Process::subprocessSignal()
		{
			return mSubProcessStarted;
		}

		SignalSlot::SignalStub<float>& Process::ratioSignal()
		{
			return mRatioChanged;
		}
	}
}
