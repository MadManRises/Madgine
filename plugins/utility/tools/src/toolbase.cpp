#include "toolslib.h"

#include "toolbase.h"

#include "Modules/reflection/classname.h"

namespace Engine
{
	namespace Tools
	{
		ToolBase::ToolBase(ImGuiRoot& root) :
		mRoot(root),
		mVisible(false)
		{
		}

		void ToolBase::render()
		{
		}

		void ToolBase::update()
		{
			if (mVisible)
				render();
		}

		bool ToolBase::isVisible()
		{
			return mVisible;
		}

		void ToolBase::setVisible(bool v)
		{
			mVisible = v;
		}

		ImGuiRoot& ToolBase::root()
		{
			return mRoot;
		}
	}
}

RegisterType(Engine::Tools::ToolBase);