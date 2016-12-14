#pragma once

#include "scope.h"

namespace Engine {
namespace Scripting {

class MADGINE_EXPORT RefScope : public Scope
{
public:
    RefScope(RefScopeTopLevelSerializableUnit *topLevel);
    virtual ~RefScope();

    void ref();
    void unref();

private:
    size_t mRefCount;

	RefScopeTopLevelSerializableUnit *mTopLevel;
};



} // namespace Scripting
} // namespace Core


