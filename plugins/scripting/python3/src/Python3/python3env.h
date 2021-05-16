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

            void execute(const std::string_view &command);

            static void lock(std::streambuf *buf);
            static std::streambuf *unlock();

            size_t totalRefCount();
        };

    }
}
}

RegisterType(Engine::Scripting::Python3::Python3Environment)