#pragma once

#include "widget.h"


namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT SceneWindow : public virtual Widget
		{
		public:			
			using Widget::Widget;
			virtual ~SceneWindow() = default;

			virtual void setCamera(Scene::Camera *camera) = 0;
			
		};
	}
}
