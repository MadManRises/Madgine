#pragma once

#include "../tool.h"

namespace Engine {
	namespace Tools {

		class Profiler : public Tool<Profiler> 
		{
		public:
			Profiler(ImGuiRoot &root);

			virtual void render() override;

			const char* key() override;

			

		private:
			Debug::Profiler::Profiler &mProfiler;
		};

	}
}
