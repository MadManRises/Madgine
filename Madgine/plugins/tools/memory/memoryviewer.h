#pragma once
#include "../tool.h"

namespace Engine
{
	namespace Tools
	{
		

		class MemoryViewer : public Tool<MemoryViewer>
		{
		public:
			MemoryViewer(ImGuiRoot &root);

			virtual void render() override;

			const char* key() override;

		private:
			Core::MemoryTracker &mTracker;
		};

	}
}
