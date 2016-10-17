#include "libinclude.h"
#include "fieldaccessor.h"
#include "Scripting/Types/globalscope.h"

namespace Engine{
namespace Scripting{


	ScopeFieldAccessor::ScopeFieldAccessor(Scope &scope, const std::string &fieldName) :
		mScope(scope),
		mFieldName(fieldName)
	{

	}


GlobalFieldAccessor::GlobalFieldAccessor(const std::string &fieldName) :
	ScopeFieldAccessor(GlobalScope::getSingleton(), fieldName)
{

}

LevelFieldAccessor::LevelFieldAccessor(const std::string &fieldName) :
	ScopeFieldAccessor(*GlobalScope::getSingleton().level(), fieldName) {

}





}
}

