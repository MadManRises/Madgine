#pragma once

#include "window.h"


namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT SceneWindow : public virtual Window
		{
		public:			
			using Window::Window;
			virtual ~SceneWindow() = default;

			virtual void setCamera(Scene::Camera *camera) = 0;
			
		};
	}
}
