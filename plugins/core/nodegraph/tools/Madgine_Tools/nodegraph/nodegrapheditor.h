#pragma once

#include "toolbase.h"
#include "toolscollector.h"

#include "Madgine/nodegraph/nodegraphloader.h"
#include "Madgine/nodegraph/pins.h"

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

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        std::string_view key() const override;

        void save();
        void load(std::string_view name);
        void create(const Filesystem::Path &path);
        std::string_view getCurrentName() const;

    protected:
        bool onSave(std::string_view view, ed::SaveReasonFlags reason);
        size_t onLoad(char *data);

        Filesystem::Path layoutPath() const;

        void createEditor();

        void queryLink();

        void verify();

    private:
        std::unique_ptr<ed::EditorContext, void (*)(ed::EditorContext *)> mEditor = { nullptr, nullptr };
        bool mHierarchyVisible;
        bool mNodeDetailsVisible;

        NodeGraph::NodeGraphLoader::HandleType mGraphHandle;
        NodeGraph::NodeGraph mGraph;

        struct NodeMessages {
            std::vector<std::string> mErrorMessages;
            std::vector<std::string> mWarningMessages;
        };
        std::map<NodeGraph::NodeBase *, NodeMessages> mNodeMessages;

        IndexType<uint32_t> mSelectedNodeIndex;

        Filesystem::Path mDirBuffer, mSelectionBuffer;
        std::string mSelectionTargetBuffer;

        Vector2 mPopupPosition;

        std::optional<NodeGraph::PinDesc> mDragPin;
        std::optional<ExtendedValueTypeDesc> mDragType;
        uint32_t mDragMask;

        bool mSaveQueued = false;
        bool mIsDirty = false;
        bool mInitialLoad = false;
    };

}
}

RegisterType(Engine::Tools::NodeGraphEditor);