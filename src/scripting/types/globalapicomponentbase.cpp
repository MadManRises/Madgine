#include "../../baselib.h"

#include "globalapicomponentbase.h"


namespace Engine
{
	namespace Scripting
	{
		bool GlobalAPIComponentBase::init()
		{
			return MadgineObject::init();
		}

		void GlobalAPIComponentBase::finalize()
		{
			MadgineObject::finalize();
		}

		void GlobalAPIComponentBase::clear()
		{
		}

		void GlobalAPIComponentBase::update()
		{
		}
	}
}
