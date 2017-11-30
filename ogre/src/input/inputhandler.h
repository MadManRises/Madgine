#pragma once

namespace Engine
{
	namespace Input
	{
		class InputHandler
		{
		public:
			virtual ~InputHandler() = default;

			virtual void update() = 0;
		};
	}
}
