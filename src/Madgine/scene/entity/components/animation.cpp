#include "../../../baselib.h"

#include "animation.h"



namespace Engine
{




	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Animation, Animation);

			void Animation::resetDefaultAnimation()
			{
				setDefaultAnimation("");
			}

		}
	}
}
