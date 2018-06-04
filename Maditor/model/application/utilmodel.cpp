#include "maditormodellib.h"

#include "utilmodel.h"

namespace Maditor {
	namespace Model {



		UtilModel::UtilModel()
		{
		}

		ProfilerModel * UtilModel::profiler()
		{
			return mProfiler.ptr();
		}

		StatsModel * UtilModel::stats()
		{
			return mStats.ptr();
		}

		void UtilModel::reset()
		{
			mProfiler->reset();
			mStats->reset();
		}

	}
}
