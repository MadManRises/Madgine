#pragma once

#include "Scripting/Datatypes/argumentlist.h"
#include "Scripting\Datatypes\valuetype.h"
#include "api.h"
#include "story.h"

namespace Engine {
namespace Scripting {


template <class T>
class GlobalAPI : public API<T> {
public:
	GlobalAPI() :
		mStory(&Story::getSingleton()) {
		mStory->addAPI(this);
	}
	virtual ~GlobalAPI() {
		mStory->removeAPI(this);
	}

	bool callGlobalMethod(const std::string &name, const ArgumentList &args = {}) {
		assert(mStory);
		return mStory->callMethodCatch(name, args);
	}

protected:
	Story *story() {
		return mStory;
	}

private:
	Story *mStory;
};




} // namespace Scripting
} // namespace Core