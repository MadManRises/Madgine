#pragma once

#include "../tool.h"


namespace Engine
{
	namespace Tools
	{

		class TestTool : public Tool<TestTool>
		{
		public:

			TestTool(ImGuiRoot &root);

			virtual void render() override;
			virtual void update() override;

			const char* key() const override;

			void logTest();
            void logValue(const ValueType &v);

		private:

		};

	}
}