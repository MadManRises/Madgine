//------------------------------------------------------------------------------
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------
#ifndef __IMGUI_NODE_EDITOR_H__
#define __IMGUI_NODE_EDITOR_H__
#pragma once

#include "Generic/commonlib.h"
#if defined(imgui_node_editor_EXPORTS)
#    define IMGUI_NODE_EDITOR_EXPORT DLL_EXPORT
#else
#    define IMGUI_NODE_EDITOR_EXPORT DLL_IMPORT
#endif

//------------------------------------------------------------------------------
#include "imgui/imgui.h"
#include <cstdint> // std::uintXX_t
#include <utility> // std::move

//------------------------------------------------------------------------------
namespace ax {
namespace NodeEditor {

    //------------------------------------------------------------------------------
    struct NodeId;
    struct LinkId;
    struct PinId;

    //------------------------------------------------------------------------------
    enum class SaveReasonFlags : uint32_t {
        None = 0x00000000,
        Navigation = 0x00000001,
        Position = 0x00000002,
        Size = 0x00000004,
        Selection = 0x00000008,
        User = 0x00000010
    };

    inline SaveReasonFlags operator|(SaveReasonFlags lhs, SaveReasonFlags rhs) { return static_cast<SaveReasonFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)); }
    inline SaveReasonFlags operator&(SaveReasonFlags lhs, SaveReasonFlags rhs) { return static_cast<SaveReasonFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)); }

    using ConfigSaveSettings = bool (*)(const char *data, size_t size, SaveReasonFlags reason, void *userPointer);
    using ConfigLoadSettings = size_t (*)(char *data, void *userPointer);

    using ConfigSaveNodeSettings = bool (*)(NodeId nodeId, const char *data, size_t size, SaveReasonFlags reason, void *userPointer);
    using ConfigLoadNodeSettings = size_t (*)(NodeId nodeId, char *data, void *userPointer);

    using ConfigSession = void (*)(void *userPointer);

    struct Config {
        const char *SettingsFile;
        ConfigSession BeginSaveSession;
        ConfigSession EndSaveSession;
        ConfigSaveSettings SaveSettings;
        ConfigLoadSettings LoadSettings;
        ConfigSaveNodeSettings SaveNodeSettings;
        ConfigLoadNodeSettings LoadNodeSettings;
        void *UserPointer;

        Config()
            : SettingsFile("NodeEditor.json")
            , BeginSaveSession(nullptr)
            , EndSaveSession(nullptr)
            , SaveSettings(nullptr)
            , LoadSettings(nullptr)
            , SaveNodeSettings(nullptr)
            , LoadNodeSettings(nullptr)
            , UserPointer(nullptr)
        {
        }
    };

    //------------------------------------------------------------------------------
    enum class PinKind {
        Input,
        Output
    };

    //------------------------------------------------------------------------------
    enum StyleColor {
        StyleColor_Bg,
        StyleColor_Grid,
        StyleColor_NodeBg,
        StyleColor_NodeBorder,
        StyleColor_HovNodeBorder,
        StyleColor_SelNodeBorder,
        StyleColor_NodeSelRect,
        StyleColor_NodeSelRectBorder,
        StyleColor_HovLinkBorder,
        StyleColor_SelLinkBorder,
        StyleColor_LinkSelRect,
        StyleColor_LinkSelRectBorder,
        StyleColor_PinRect,
        StyleColor_PinRectBorder,
        StyleColor_Flow,
        StyleColor_FlowMarker,
        StyleColor_GroupBg,
        StyleColor_GroupBorder,

        StyleColor_Count
    };

    enum StyleVar {
        StyleVar_NodePadding,
        StyleVar_NodeRounding,
        StyleVar_NodeBorderWidth,
        StyleVar_HoveredNodeBorderWidth,
        StyleVar_SelectedNodeBorderWidth,
        StyleVar_PinRounding,
        StyleVar_PinBorderWidth,
        StyleVar_LinkStrength,
        StyleVar_SourceDirection,
        StyleVar_TargetDirection,
        StyleVar_ScrollDuration,
        StyleVar_FlowMarkerDistance,
        StyleVar_FlowSpeed,
        StyleVar_FlowDuration,
        StyleVar_PivotAlignment,
        StyleVar_PivotSize,
        StyleVar_PivotScale,
        StyleVar_PinCorners,
        StyleVar_PinRadius,
        StyleVar_PinArrowSize,
        StyleVar_PinArrowWidth,
        StyleVar_GroupRounding,
        StyleVar_GroupBorderWidth,
    };

    struct Style {
        ImVec4 NodePadding;
        float NodeRounding;
        float NodeBorderWidth;
        float HoveredNodeBorderWidth;
        float SelectedNodeBorderWidth;
        float PinRounding;
        float PinBorderWidth;
        float LinkStrength;
        ImVec2 SourceDirection;
        ImVec2 TargetDirection;
        float ScrollDuration;
        float FlowMarkerDistance;
        float FlowSpeed;
        float FlowDuration;
        ImVec2 PivotAlignment;
        ImVec2 PivotSize;
        ImVec2 PivotScale;
        float PinCorners;
        float PinRadius;
        float PinArrowSize;
        float PinArrowWidth;
        float GroupRounding;
        float GroupBorderWidth;
        ImVec4 Colors[StyleColor_Count];

        Style()
        {
            NodePadding = ImVec4(8, 8, 8, 8);
            NodeRounding = 12.0f;
            NodeBorderWidth = 1.5f;
            HoveredNodeBorderWidth = 3.5f;
            SelectedNodeBorderWidth = 3.5f;
            PinRounding = 4.0f;
            PinBorderWidth = 0.0f;
            LinkStrength = 100.0f;
            SourceDirection = ImVec2(1.0f, 0.0f);
            TargetDirection = ImVec2(-1.0f, 0.0f);
            ScrollDuration = 0.35f;
            FlowMarkerDistance = 30.0f;
            FlowSpeed = 150.0f;
            FlowDuration = 2.0f;
            PivotAlignment = ImVec2(0.5f, 0.5f);
            PivotSize = ImVec2(0.0f, 0.0f);
            PivotScale = ImVec2(1, 1);
            PinCorners = ImDrawCornerFlags_All;
            PinRadius = 0.0f;
            PinArrowSize = 0.0f;
            PinArrowWidth = 0.0f;
            GroupRounding = 6.0f;
            GroupBorderWidth = 1.0f;

            Colors[StyleColor_Bg] = ImColor(60, 60, 70, 200);
            Colors[StyleColor_Grid] = ImColor(120, 120, 120, 40);
            Colors[StyleColor_NodeBg] = ImColor(32, 32, 32, 200);
            Colors[StyleColor_NodeBorder] = ImColor(255, 255, 255, 96);
            Colors[StyleColor_HovNodeBorder] = ImColor(50, 176, 255, 255);
            Colors[StyleColor_SelNodeBorder] = ImColor(255, 176, 50, 255);
            Colors[StyleColor_NodeSelRect] = ImColor(5, 130, 255, 64);
            Colors[StyleColor_NodeSelRectBorder] = ImColor(5, 130, 255, 128);
            Colors[StyleColor_HovLinkBorder] = ImColor(50, 176, 255, 255);
            Colors[StyleColor_SelLinkBorder] = ImColor(255, 176, 50, 255);
            Colors[StyleColor_LinkSelRect] = ImColor(5, 130, 255, 64);
            Colors[StyleColor_LinkSelRectBorder] = ImColor(5, 130, 255, 128);
            Colors[StyleColor_PinRect] = ImColor(60, 180, 255, 100);
            Colors[StyleColor_PinRectBorder] = ImColor(60, 180, 255, 128);
            Colors[StyleColor_Flow] = ImColor(255, 128, 64, 255);
            Colors[StyleColor_FlowMarker] = ImColor(255, 128, 64, 255);
            Colors[StyleColor_GroupBg] = ImColor(0, 0, 0, 160);
            Colors[StyleColor_GroupBorder] = ImColor(255, 255, 255, 32);
        }
    };

    //------------------------------------------------------------------------------
    struct EditorContext;

    //------------------------------------------------------------------------------
    IMGUI_NODE_EDITOR_EXPORT void SetCurrentEditor(EditorContext *ctx);
    IMGUI_NODE_EDITOR_EXPORT EditorContext *GetCurrentEditor();
    IMGUI_NODE_EDITOR_EXPORT EditorContext *CreateEditor(const Config *config = nullptr);
    IMGUI_NODE_EDITOR_EXPORT void DestroyEditor(EditorContext *ctx);

    IMGUI_NODE_EDITOR_EXPORT Style &GetStyle();
    IMGUI_NODE_EDITOR_EXPORT const char *GetStyleColorName(StyleColor colorIndex);

    IMGUI_NODE_EDITOR_EXPORT void PushStyleColor(StyleColor colorIndex, const ImVec4 &color);
    IMGUI_NODE_EDITOR_EXPORT void PopStyleColor(int count = 1);

    IMGUI_NODE_EDITOR_EXPORT void PushStyleVar(StyleVar varIndex, float value);
    IMGUI_NODE_EDITOR_EXPORT void PushStyleVar(StyleVar varIndex, const ImVec2 &value);
    IMGUI_NODE_EDITOR_EXPORT void PushStyleVar(StyleVar varIndex, const ImVec4 &value);
    IMGUI_NODE_EDITOR_EXPORT void PopStyleVar(int count = 1);

    IMGUI_NODE_EDITOR_EXPORT void Begin(const char *id, const ImVec2 &size = ImVec2(0, 0));
    IMGUI_NODE_EDITOR_EXPORT void End();

    IMGUI_NODE_EDITOR_EXPORT void BeginNode(NodeId id);
    IMGUI_NODE_EDITOR_EXPORT void BeginPin(PinId id, PinKind kind);
    IMGUI_NODE_EDITOR_EXPORT void PinRect(const ImVec2 &a, const ImVec2 &b);
    IMGUI_NODE_EDITOR_EXPORT void PinPivotRect(const ImVec2 &a, const ImVec2 &b);
    IMGUI_NODE_EDITOR_EXPORT void PinPivotSize(const ImVec2 &size);
    IMGUI_NODE_EDITOR_EXPORT void PinPivotScale(const ImVec2 &scale);
    IMGUI_NODE_EDITOR_EXPORT void PinPivotAlignment(const ImVec2 &alignment);
    IMGUI_NODE_EDITOR_EXPORT void EndPin();
    IMGUI_NODE_EDITOR_EXPORT void Group(const ImVec2 &size);
    IMGUI_NODE_EDITOR_EXPORT void EndNode();

    IMGUI_NODE_EDITOR_EXPORT bool BeginGroupHint(NodeId nodeId);
    IMGUI_NODE_EDITOR_EXPORT ImVec2 GetGroupMin();
    IMGUI_NODE_EDITOR_EXPORT ImVec2 GetGroupMax();
    IMGUI_NODE_EDITOR_EXPORT ImDrawList *GetHintForegroundDrawList();
    IMGUI_NODE_EDITOR_EXPORT ImDrawList *GetHintBackgroundDrawList();
    IMGUI_NODE_EDITOR_EXPORT void EndGroupHint();

    // TODO: Add a way to manage node background channels
    IMGUI_NODE_EDITOR_EXPORT ImDrawList *GetNodeBackgroundDrawList(NodeId nodeId);

    IMGUI_NODE_EDITOR_EXPORT bool Link(LinkId id, PinId startPinId, PinId endPinId, const ImVec4 &color = ImVec4(1, 1, 1, 1), float thickness = 1.0f);

    IMGUI_NODE_EDITOR_EXPORT void Flow(LinkId linkId);

    IMGUI_NODE_EDITOR_EXPORT bool BeginCreate(const ImVec4 &color = ImVec4(1, 1, 1, 1), float thickness = 1.0f);
    IMGUI_NODE_EDITOR_EXPORT bool QueryNewLink(PinId *startId, PinId *endId);
    IMGUI_NODE_EDITOR_EXPORT bool QueryNewLink(PinId *startId, PinId *endId, const ImVec4 &color, float thickness = 1.0f);
    IMGUI_NODE_EDITOR_EXPORT bool QueryNewNode(PinId *pinId);
    IMGUI_NODE_EDITOR_EXPORT bool QueryNewNode(PinId *pinId, const ImVec4 &color, float thickness = 1.0f);
    IMGUI_NODE_EDITOR_EXPORT bool AcceptNewItem();
    IMGUI_NODE_EDITOR_EXPORT bool AcceptNewItem(const ImVec4 &color, float thickness = 1.0f);
    IMGUI_NODE_EDITOR_EXPORT void RejectNewItem();
    IMGUI_NODE_EDITOR_EXPORT void RejectNewItem(const ImVec4 &color, float thickness = 1.0f);
    IMGUI_NODE_EDITOR_EXPORT void EndCreate();

    IMGUI_NODE_EDITOR_EXPORT bool BeginDelete();
    IMGUI_NODE_EDITOR_EXPORT bool QueryDeletedLink(LinkId *linkId, PinId *startId = nullptr, PinId *endId = nullptr);
    IMGUI_NODE_EDITOR_EXPORT bool QueryDeletedNode(NodeId *nodeId);
    IMGUI_NODE_EDITOR_EXPORT bool AcceptDeletedItem();
    IMGUI_NODE_EDITOR_EXPORT void RejectDeletedItem();
    IMGUI_NODE_EDITOR_EXPORT void EndDelete();

    IMGUI_NODE_EDITOR_EXPORT void SetNodePosition(NodeId nodeId, const ImVec2 &editorPosition);
    IMGUI_NODE_EDITOR_EXPORT ImVec2 GetNodePosition(NodeId nodeId);
    IMGUI_NODE_EDITOR_EXPORT ImVec2 GetNodeSize(NodeId nodeId);
    IMGUI_NODE_EDITOR_EXPORT void CenterNodeOnScreen(NodeId nodeId);

    IMGUI_NODE_EDITOR_EXPORT void RestoreNodeState(NodeId nodeId);

    IMGUI_NODE_EDITOR_EXPORT void Suspend();
    IMGUI_NODE_EDITOR_EXPORT void Resume();
    IMGUI_NODE_EDITOR_EXPORT bool IsSuspended();

    IMGUI_NODE_EDITOR_EXPORT bool IsActive();

    IMGUI_NODE_EDITOR_EXPORT bool HasSelectionChanged();
    IMGUI_NODE_EDITOR_EXPORT int GetSelectedObjectCount();
    IMGUI_NODE_EDITOR_EXPORT int GetSelectedNodes(NodeId *nodes, int size);
    IMGUI_NODE_EDITOR_EXPORT int GetSelectedLinks(LinkId *links, int size);
    IMGUI_NODE_EDITOR_EXPORT void ClearSelection();
    IMGUI_NODE_EDITOR_EXPORT void SelectNode(NodeId nodeId, bool append = false);
    IMGUI_NODE_EDITOR_EXPORT void SelectLink(LinkId linkId, bool append = false);
    IMGUI_NODE_EDITOR_EXPORT void DeselectNode(NodeId nodeId);
    IMGUI_NODE_EDITOR_EXPORT void DeselectLink(LinkId linkId);

    IMGUI_NODE_EDITOR_EXPORT bool DeleteNode(NodeId nodeId);
    IMGUI_NODE_EDITOR_EXPORT bool DeleteLink(LinkId linkId);

    IMGUI_NODE_EDITOR_EXPORT void NavigateToContent(float duration = -1);
    IMGUI_NODE_EDITOR_EXPORT void NavigateToSelection(bool zoomIn = false, float duration = -1);

    IMGUI_NODE_EDITOR_EXPORT bool ShowNodeContextMenu(NodeId *nodeId);
    IMGUI_NODE_EDITOR_EXPORT bool ShowPinContextMenu(PinId *pinId);
    IMGUI_NODE_EDITOR_EXPORT bool ShowLinkContextMenu(LinkId *linkId);
    IMGUI_NODE_EDITOR_EXPORT bool ShowBackgroundContextMenu();

    IMGUI_NODE_EDITOR_EXPORT void EnableShortcuts(bool enable);
    IMGUI_NODE_EDITOR_EXPORT bool AreShortcutsEnabled();

    IMGUI_NODE_EDITOR_EXPORT bool BeginShortcut();
    IMGUI_NODE_EDITOR_EXPORT bool AcceptCut();
    IMGUI_NODE_EDITOR_EXPORT bool AcceptCopy();
    IMGUI_NODE_EDITOR_EXPORT bool AcceptPaste();
    IMGUI_NODE_EDITOR_EXPORT bool AcceptDuplicate();
    IMGUI_NODE_EDITOR_EXPORT bool AcceptCreateNode();
    IMGUI_NODE_EDITOR_EXPORT int GetActionContextSize();
    IMGUI_NODE_EDITOR_EXPORT int GetActionContextNodes(NodeId *nodes, int size);
    IMGUI_NODE_EDITOR_EXPORT int GetActionContextLinks(LinkId *links, int size);
    IMGUI_NODE_EDITOR_EXPORT void EndShortcut();

    IMGUI_NODE_EDITOR_EXPORT float GetCurrentZoom();

    IMGUI_NODE_EDITOR_EXPORT NodeId GetDoubleClickedNode();
    IMGUI_NODE_EDITOR_EXPORT PinId GetDoubleClickedPin();
    IMGUI_NODE_EDITOR_EXPORT LinkId GetDoubleClickedLink();
    IMGUI_NODE_EDITOR_EXPORT bool IsBackgroundClicked();
    IMGUI_NODE_EDITOR_EXPORT bool IsBackgroundDoubleClicked();

    IMGUI_NODE_EDITOR_EXPORT bool PinHadAnyLinks(PinId pinId);

    IMGUI_NODE_EDITOR_EXPORT ImVec2 GetScreenSize();
    IMGUI_NODE_EDITOR_EXPORT ImVec2 ScreenToCanvas(const ImVec2 &pos);
    IMGUI_NODE_EDITOR_EXPORT ImVec2 CanvasToScreen(const ImVec2 &pos);

    //------------------------------------------------------------------------------
    namespace Details {

        template <typename T, typename Tag>
        struct SafeType {
            SafeType(T t)
                : m_Value(std::move(t))
            {
            }

            SafeType(const SafeType &) = default;

            template <typename T2, typename Tag2>
            SafeType(
                const SafeType<
                    typename std::enable_if<!std::is_same<T, T2>::value, T2>::type,
                    typename std::enable_if<!std::is_same<Tag, Tag2>::value, Tag2>::type> &)
                = delete;

            SafeType &operator=(const SafeType &) = default;

            explicit operator T() const { return Get(); }

            T Get() const { return m_Value; }

        private:
            T m_Value;
        };

        template <typename Tag>
        struct SafePointerType
            : SafeType<uintptr_t, Tag> {
            static const Tag Invalid;

            using SafeType<uintptr_t, Tag>::SafeType;

            SafePointerType()
                : SafePointerType(Invalid)
            {
            }

            template <typename T = void>
            explicit SafePointerType(T *ptr)
                : SafePointerType(reinterpret_cast<uintptr_t>(ptr))
            {
            }
            template <typename T = void>
            T *AsPointer() const { return reinterpret_cast<T *>(this->Get()); }

            explicit operator bool() const { return *this != Invalid; }
        };

        template <typename Tag>
        const Tag SafePointerType<Tag>::Invalid = { 0 };

        template <typename Tag>
        inline bool operator==(const SafePointerType<Tag> &lhs, const SafePointerType<Tag> &rhs)
        {
            return lhs.Get() == rhs.Get();
        }

        template <typename Tag>
        inline bool operator!=(const SafePointerType<Tag> &lhs, const SafePointerType<Tag> &rhs)
        {
            return lhs.Get() != rhs.Get();
        }

    } // namespace Details

    struct NodeId final : Details::SafePointerType<NodeId> {
        using SafePointerType::SafePointerType;
    };

    struct LinkId final : Details::SafePointerType<LinkId> {
        using SafePointerType::SafePointerType;
    };

    struct PinId final : Details::SafePointerType<PinId> {
        using SafePointerType::SafePointerType;
    };

    //------------------------------------------------------------------------------
} // namespace Editor
} // namespace ax

//------------------------------------------------------------------------------
#endif // __IMGUI_NODE_EDITOR_H__