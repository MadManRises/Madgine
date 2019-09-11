#pragma once

#include "../toolscollector.h"

namespace Engine {
	namespace Tools {

		class Profiler : public Tool<Profiler> 
		{
		public:
			Profiler(ImRoot &root);

			virtual void render() override;

			const char* key() const override;

			

		private:
			Debug::Profiler::Profiler &mProfiler;
		};

	}
}
