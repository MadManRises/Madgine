#pragma once

namespace Engine
{
	namespace Serialize
	{
		
		enum Operations
		{
			INSERT_ITEM = 0x1,
			REMOVE_ITEM = 0x2,
			REMOVE_RANGE = 0x3,
			RESET = 0x4,


			MASK = 0x0F,
			ACCEPT = 0x10,
			REJECT = 0x20,

			BEFORE = 0x0,
			AFTER = 0x10
		};


	}
}