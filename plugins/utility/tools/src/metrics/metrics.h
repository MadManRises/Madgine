#pragma once

#include "../tool.h"

#include "Interfaces/debug/history.h"

namespace Engine
{
	namespace Tools
	{

		class Metrics : public Tool<Metrics>
		{
		public:

			Metrics(ImGuiRoot &root);

			virtual void render() override;
			virtual void update() override;

			const char* key() override;

		private:
			Debug::History<float, 120> mFramesPerSecond;

			Debug::History<float, 100> mFramesTrend;
			float mTimeBank;

		};

	}
}

RegisterClass(Engine::Tools::Metrics);