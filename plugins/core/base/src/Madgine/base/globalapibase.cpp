#include "../baselib.h"

#include "globalapibase.h"

#include "application.h"

#include "Meta/keyvalue/metatable_impl.h"

namespace Engine {
namespace Base {
    GlobalAPIBase::GlobalAPIBase(Application &app)
        : mApp(app)
    {
    }

    Threading::TaskQueue *GlobalAPIBase::taskQueue() const
    {
        return mApp.taskQueue();
    }

    Application &GlobalAPIBase::app()
    {
        return mApp;
    }

    Threading::Task<bool> GlobalAPIBase::init()
    {
        co_return true;
    }

    Threading::Task<void> GlobalAPIBase::finalize()
    {
        co_return;
    }

    GlobalAPIBase &GlobalAPIBase::getGlobalAPIComponent(size_t i)
    {
        return mApp.getGlobalAPIComponent(i);
    }

}
}

METATABLE_BEGIN(Engine::Base::GlobalAPIBase)
METATABLE_END(Engine::Base::GlobalAPIBase)
