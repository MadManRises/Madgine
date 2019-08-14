#pragma once

namespace Engine
{
	namespace Serialize
	{
		
		enum Operations
		{
			REMOVE_RANGE = 0x3,
			RESET = 0x4,


			MASK = 0x0F,
			ACCEPT = 0x10,
			REJECT = 0x20,
		};


	}
}