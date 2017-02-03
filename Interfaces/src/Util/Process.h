#pragma once

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

			void addListener(ProcessListener *listener);
			void removeListener(ProcessListener *listener);

		private:
			float mRatio;
			
			std::list<float> mStepSizes;
			std::list<size_t> mSubProcessCounts;
			std::list<size_t> mSubProcessIndices;

			std::list<ProcessListener *> mListeners;
		};

	}
}


