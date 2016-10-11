#include "libinclude.h"
#include "fieldaccessor.h"
#include "Scripting/Types/story.h"

namespace Engine{
namespace Scripting{


	ScopeFieldAccessor::ScopeFieldAccessor(Scope &scope, const std::string &fieldName) :
		mScope(scope),
		mFieldName(fieldName)
	{

	}


GlobalFieldAccessor::GlobalFieldAccessor(const std::string &fieldName) :
	ScopeFieldAccessor(Story::getSingleton(), fieldName)
{

}

LevelFieldAccessor::LevelFieldAccessor(const std::string &fieldName) :
	ScopeFieldAccessor(*Story::getSingleton().level(), fieldName) {

}





}
}

