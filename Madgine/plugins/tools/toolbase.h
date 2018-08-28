#pragma once

namespace Engine
{
	namespace Tools
	{
		
		class ToolBase
		{
		public:
			ToolBase(ImGuiRoot &root);

			virtual void render();

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