#pragma once

#include "Madgine/app/globalapibase.h"
#include "Madgine/app/globalapicollector.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "python3streamredirect.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3Environment : App::GlobalAPI<Python3Environment> {
            Python3Environment(App::Application &app);
            ~Python3Environment();

            virtual Threading::Task<bool> init() override;
            virtual Threading::Task<void> finalize() override;

            void execute(std::string_view command);

            static PyGILState_STATE lock(std::streambuf *buf);
            static std::streambuf *unlock(PyGILState_STATE state);

            static size_t totalRefCount();
        };

    }
}
}

RegisterType(Engine::Scripting::Python3::Python3Environment)