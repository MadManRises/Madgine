#pragma once

#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "python3debugger.h"

#include "Generic/execution/virtualsender.h"
#include "Generic/genericresult.h"

#include "Madgine/debug/debuggablesender.h"

#include "Meta/keyvalue/valuetype.h"

#include "util/pyexecution.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3Environment : Root::RootComponent<Python3Environment> {
            Python3Environment(Root::Root &root);
            ~Python3Environment();

            std::string_view key() const override;

            ExecutionSender execute(std::string_view command, Closure<void(std::string_view)> out = {});

            static PyGILState_STATE lock();
            static Closure<void(std::string_view)> unlock(PyGILState_STATE state);
            static void lock(Closure<void(std::string_view)> out, std::stop_token st);
            static std::pair<Closure<void(std::string_view)>, std::stop_token> unlock();

            static size_t totalRefCount();

        private:
            Python3Debugger mDebugger;
        };

    }
}
}

REGISTER_TYPE(Engine::Scripting::Python3::Python3Environment)