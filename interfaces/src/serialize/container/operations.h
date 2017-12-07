#pragma once

namespace Engine
{
	namespace Serialize
	{
		
		enum Operations
		{
			INSERT_ITEM = 0x1,
			REMOVE_ITEM = 0x2,
			RESET = 0x3,


			MASK = 0x0F,
			ACCEPT = 0x10,
			REJECT = 0x20,

			BEFORE = 0x0,
			AFTER = 0x10
		};


	}
}