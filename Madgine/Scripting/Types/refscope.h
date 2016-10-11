#pragma once

#include "scope.h"

namespace Engine {
namespace Scripting {

class MADGINE_EXPORT RefScope : public Scope
{
public:
    RefScope();
    virtual ~RefScope();

    void ref();
    void unref();

private:
    size_t mRefCount;

};


template <class T>
class ScopeRef {
public:
	ScopeRef(){
		mPtr = new T();
		mPtr->ref();
	}

	~ScopeRef() {
		mPtr->unref();
	}

	operator T &() {
		return *mPtr;
	}

private:
	T *mPtr;
};

} // namespace Scripting
} // namespace Core


