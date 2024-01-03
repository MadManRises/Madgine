#pragma once

#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "python3debugger.h"

#include "Generic/execution/virtualsender.h"
#include "Generic/genericresult.h"


namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3Environment : Root::RootComponent<Python3Environment> {
            Python3Environment(Root::Root &root);
            ~Python3Environment();

            std::string_view key() const override;

            void execute(KeyValueReceiver &receiver, std::string_view command, Lambda<void(std::string_view)> out = {});
            void execute(KeyValueReceiver &receiver, PyCodeObject *code);

            auto execute(std::string_view command, Lambda<void(std::string_view)> out = {})
            {
                auto f = LIFT(execute, this);
                return Execution::make_virtual_sender<Execution::SimpleState<decltype(f), std::tuple<std::string_view, Lambda<void(std::string_view)>>, KeyValueReceiver>, GenericResult, ArgumentList>(std::move(f), std::make_tuple(command, std::move(out)));
            }

            static PyGILState_STATE lock();
            static Lambda<void(std::string_view)> unlock(PyGILState_STATE state);
            static void lock(Lambda<void(std::string_view)> out);
            static Lambda<void(std::string_view)> unlock();

            static size_t totalRefCount();

        private:
            Python3Debugger mDebugger;
        };

    }
}
}

REGISTER_TYPE(Engine::Scripting::Python3::Python3Environment)