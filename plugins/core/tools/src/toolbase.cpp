#include "toolslib.h"

#include "toolbase.h"

#include "renderer/imroot.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

namespace Engine {
namespace Tools {
    ToolBase::ToolBase(ImRoot &root)
        : mRoot(root)
    {
    }

    void ToolBase::render()
    {
    }

    void ToolBase::renderMenu()
    {
    }

    void ToolBase::renderStatus()
    {
    }

    void ToolBase::update()
    {
        if (mVisible)
            render();
    }

    bool ToolBase::isVisible()
    {
        return mVisible;
    }

    void ToolBase::setVisible(bool v)
    {
        mVisible = v;
    }

    ToolBase &ToolBase::getToolComponent(size_t index)
    {
        return mRoot.getToolComponent(index);
    }

    ImRoot &ToolBase::root()
    {
        return mRoot;
    }
       
    Threading::TaskQueue *ToolBase::taskQueue() const
    {
        return mRoot.taskQueue();
    }

    bool ToolBase::init()
    {
        return true;
    }

    void ToolBase::finalize()
    {
    }
}
}

METATABLE_BEGIN(Engine::Tools::ToolBase)
PROPERTY(visible, isVisible, setVisible)
METATABLE_END(Engine::Tools::ToolBase)

SERIALIZETABLE_BEGIN(Engine::Tools::ToolBase)
FIELD(mVisible)
SERIALIZETABLE_END(Engine::Tools::ToolBase)
