#include "maditorinclude.h"

#include "CmakeSubProject.h"


namespace Maditor {
	namespace Model {
		namespace Generator {

			CmakeSubProject::CmakeSubProject(CmakeGenerator * parent, const QString & name) :
				CmakeGenerator(name),
				mParent(parent)
			{
				mParent->addSubProject(this);
			}

			QString CmakeSubProject::root()
			{
				return mParent->root() + name() + "/";
			}
		}
	}
}