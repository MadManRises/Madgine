#pragma once

#include "Interfaces/reflection/classname.h"

namespace Engine
{
	namespace Tools
	{
		
		class ToolBase
		{
		public:
			ToolBase(ImGuiRoot &root);

			virtual void render();
			virtual void update();

			virtual const char *key() = 0;

			bool isVisible();
			void setVisible(bool v);

		protected:
			ImGuiRoot &root();

		private:
			ImGuiRoot &mRoot;
			bool mVisible;
		};

	}
}

RegisterClass(Engine::Tools::ToolBase);