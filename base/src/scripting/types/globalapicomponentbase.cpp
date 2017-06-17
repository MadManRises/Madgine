#include "baselib.h"

#include "globalapicomponentbase.h"
#include "scripting/types/globalscopebase.h"



namespace Engine {
	namespace Scripting {

		bool GlobalAPIComponentBase::init() {
			if (MadgineObject::init()) {
				globalScope()->addGlobal(this);
				return true;
			}
			else
				return false;
		}
		void GlobalAPIComponentBase::finalize() {
			MadgineObject::finalize();
			if (globalScope()) {
				globalScope()->removeGlobal(this);
			}
		}

		void GlobalAPIComponentBase::clear() {
		}

		void GlobalAPIComponentBase::update()
		{
		}


	}
}