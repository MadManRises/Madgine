#include "../toolslib.h"

#    include "filebrowser.h"

#    include "imgui/imgui.h"
#    include "imgui/imgui_internal.h"
#    include "imgui/imguiaddons.h"

#    include "Meta/keyvalue/metatable_impl.h"
#    include "Meta/serialize/serializetable_impl.h"

#    include "Interfaces/filesystem/api.h"

#    include "Modules/uniquecomponent/uniquecomponentcollector.h"

#    include "Madgine/core/root.h"

UNIQUECOMPONENT(Engine::Tools::FileBrowser);

METATABLE_BEGIN_BASE(Engine::Tools::FileBrowser, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::Filesystem)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::FileBrowser, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Filesystem)

namespace Engine {
namespace Tools {

    FileBrowser::FileBrowser(ImRoot &root)
        : Tool<FileBrowser>(root)
    {
    }

    void FileBrowser::render()
    {
        if (ImGui::Begin("File Browser", &mVisible)) {
            bool accepted;
            ImGui::FilePicker(&mCurrentPath, &mSelectedPath, accepted);
        }
        ImGui::End();
    }

    std::string_view FileBrowser::key() const
    {
        return "FileBrowser";
    }

}
}
