#include "../toolslib.h"

#    include "filesystem.h"

#    include "imgui/imgui.h"
#    include "imgui/imgui_internal.h"
#    include "imgui/imguiaddons.h"

#    include "Meta/keyvalue/metatable_impl.h"
#    include "Meta/serialize/serializetable_impl.h"

#    include "Interfaces/filesystem/api.h"

#    include "Modules/uniquecomponent/uniquecomponentcollector.h"

#    include "Madgine/core/root.h"

UNIQUECOMPONENT(Engine::Tools::Filesystem);

METATABLE_BEGIN_BASE(Engine::Tools::Filesystem, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::Filesystem)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::Filesystem, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Filesystem)

namespace Engine {
namespace Tools {

    Filesystem::Filesystem(ImRoot &root)
        : Tool<Filesystem>(root)
    {
    }

    void Filesystem::render()
    {
        if (ImGui::Begin("Filesystem", &mVisible)) {
            bool accepted;
            ImGui::FilePicker(&mCurrentPath, &mSelectedPath, accepted);
        }
        ImGui::End();
    }

    std::string_view Filesystem::key() const
    {
        return "Filesystem";
    }

}
}
