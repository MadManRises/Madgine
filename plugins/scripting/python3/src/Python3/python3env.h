#pragma once

#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

#include "python3streamredirect.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3Environment : Root::RootComponent<Python3Environment> {
            Python3Environment(Root::Root &root);
            ~Python3Environment();

            std::string_view key() const override;
            
            void execute(std::string_view command);

            static PyGILState_STATE lock(std::streambuf *buf);
            static std::streambuf *unlock(PyGILState_STATE state);

            static size_t totalRefCount();
        };

    }
}
}

REGISTER_TYPE(Engine::Scripting::Python3::Python3Environment)