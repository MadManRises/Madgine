#pragma once

#include "../toolscollector.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "dialogs.h"

#include "Interfaces/filesystem/path.h"

namespace Engine {

struct MadgineObjectState;

namespace Tools {

    struct MADGINE_TOOLS_EXPORT ImRoot {
        ImRoot();
        ~ImRoot();

        virtual Threading::Task<bool> init();
        virtual Threading::Task<void> finalize();

        const std::vector<std::unique_ptr<ToolBase>> &tools();
        ToolBase &getTool(size_t index);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getTool(::Engine::UniqueComponent::component_index<T>()));
        }

        void render();

        unsigned int dockSpaceId() const;

        std::stringstream mToolReadBuffer;
        ToolBase *mToolReadTool = nullptr;

        void finishToolRead();

        virtual Threading::TaskQueue *taskQueue() const = 0;

        template <typename F, typename Tuple>
        auto dialog(F &&f, Tuple &&tuple, const DialogSettings &settings = {})
        {
            return DialogSender { settings, &mDialogContainer, std::forward<F>(f), std::forward<Tuple>(tuple) };
        }

        auto directoryPicker()
        {
            return DialogSender { { .acceptText = "Open" }, &mDialogContainer, [this, path { Filesystem::Path {} }](Filesystem::Path &selected) mutable { return directoryPickerImpl(path, selected); }, std::make_tuple(Filesystem::Path {}) };
        }

        auto filePicker(bool allowNewFile = false)
        {
            return DialogSender { { .acceptText = allowNewFile ? "Save" : "Open" }, &mDialogContainer, [this, path { Filesystem::Path {} }, allowNewFile](Filesystem::Path &selected) mutable { return filePickerImpl(allowNewFile, path, selected); }, std::make_tuple(Filesystem::Path {}) };
        }

    protected:
        DialogFlags directoryPickerImpl(Filesystem::Path &path, Filesystem::Path &selected);
        DialogFlags filePickerImpl(bool allowNewFile, Filesystem::Path &path, Filesystem::Path &selected);

        unsigned int mDockSpaceId;

    private:
        ToolsContainer<std::vector<Placeholder<0>>> mCollector;

        DialogContainer mDialogContainer;
    };

}
}

REGISTER_TYPE(Engine::Tools::ImRoot)
