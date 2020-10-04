#pragma once

#include "toolscollector.h"
#include "toolbase.h"

#include "Madgine/nodegraph/nodegraphprototypeloader.h"
#include "Madgine/nodegraph/pinprototypes.h"

namespace ed = ax::NodeEditor;

namespace ax::NodeEditor {
enum class SaveReasonFlags : uint32_t;
}

namespace Engine {
namespace Tools {

    struct NodeGraphEditor : public Tool<NodeGraphEditor> {

        SERIALIZABLEUNIT(NodeGraphEditor);

        NodeGraphEditor(ImRoot &root);
        NodeGraphEditor(const NodeGraphEditor &) = delete;

        virtual bool init() override;
        virtual void finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        std::string_view key() const override;

        void save();
        void load(const std::string &name);
        void create(const Filesystem::Path &path);

    protected:
        bool onSave(const std::string_view &view, ed::SaveReasonFlags reason);
        size_t onLoad(char *data);

        Filesystem::Path layoutPath() const;

        void createEditor();

    private:
        ed::EditorContext *mEditor = nullptr;
        bool mHierarchyVisible;

        NodeGraph::NodeGraphPrototypeLoader::HandleType mGraph;

        Filesystem::Path mDirBuffer, mSelectionBuffer;
        std::string mSelectionTargetBuffer;

        ImVec2 mPopupPosition;

        std::optional<NodeGraph::PinDesc> mDragPin;
        std::optional<ExtendedValueTypeDesc> mDragType;

        bool mSaveQueued = false;
        bool mIsDirty = false;
        bool mInitialLoad = false;
    };

}
}

RegisterType(Engine::Tools::NodeGraphEditor);