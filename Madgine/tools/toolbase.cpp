#include "toolslib.h"

#include "toolbase.h"

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