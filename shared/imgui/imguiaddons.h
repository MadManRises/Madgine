#pragma once

#include "imconfig.h"
#include "imgui.h"

#include "Meta/keyvalue/valuetype_forward.h"

#include "Generic/coroutines/generator.h"

namespace ImGui {

struct ValueTypePayload;

typedef int ImGuiDragDropFlags;
typedef int ImGuiTreeNodeFlags;
typedef int ImGuiInputTextFlags;

struct IMGUI_API ValueTypeDrawer {
    static bool draw(Engine::TypedScopePtr &scope);
    static bool draw(const Engine::TypedScopePtr &scope);
    static bool draw(bool &b);
    static bool draw(const bool &b);
    static bool draw(Engine::CoWString &s);
    static bool draw(const Engine::CoWString &s);
    static bool draw(int &i);
    static bool draw(const int &i);
    static bool draw(uint64_t &i);
    static bool draw(const uint64_t &i);
    static bool draw(float &f);
    static bool draw(const float &f);
    static bool draw(Engine::Matrix3 &m);
    static bool draw(const Engine::Matrix3 &m);
    static bool draw(Engine::Matrix3 *m);
    static bool draw(const Engine::Matrix3 *m);
    static bool draw(Engine::Matrix4 &m);
    static bool draw(const Engine::Matrix4 &m);
    static bool draw(Engine::Matrix4 *m);
    static bool draw(const Engine::Matrix4 *m);
    static bool draw(Engine::Vector2 &v);
    static bool draw(const Engine::Vector2 &v);
    static bool draw(Engine::Vector3 &v);
    static bool draw(const Engine::Vector3 &v);
    static bool draw(Engine::Vector4 &v);
    static bool draw(const Engine::Vector4 &v);
    static bool draw(Engine::Vector2i &v);
    static bool draw(const Engine::Vector2i &v);
    static bool draw(Engine::Vector3i &v);
    static bool draw(const Engine::Vector3i &v);
    static bool draw(Engine::Vector4i &v);
    static bool draw(const Engine::Vector4i &v);
    static bool draw(Engine::KeyValueVirtualSequenceRange &range);
    static bool draw(const Engine::KeyValueVirtualSequenceRange &range);
    static bool draw(Engine::KeyValueVirtualAssociativeRange &range);
    static bool draw(const Engine::KeyValueVirtualAssociativeRange &range);
    static bool draw(Engine::KeyValueFunction &m);
    static bool draw(const Engine::KeyValueFunction &m);
    static bool draw(Engine::ApiFunction &m);
    static bool draw(const Engine::ApiFunction &m);
    static bool draw(Engine::BoundApiFunction &m);
    static bool draw(const Engine::BoundApiFunction &m);
    static bool draw(std::monostate &);
    static bool draw(const std::monostate &);
    static bool draw(Engine::Quaternion &q);
    static bool draw(const Engine::Quaternion &q);
    static bool draw(Engine::ObjectPtr &o);
    static bool draw(const Engine::ObjectPtr &o);
    static bool draw(Engine::Filesystem::Path &p);
    static bool draw(const Engine::Filesystem::Path &p);
    static bool draw(Engine::EnumHolder &e);
    static bool draw(const Engine::EnumHolder &e);
    static bool draw(Engine::Color3 &c);
    static bool draw(const Engine::Color3 &c);
    static bool draw(Engine::Color4 &c);
    static bool draw(const Engine::Color4 &c);
};

IMGUI_API void setPayloadStatus(std::string_view s);

IMGUI_API void Text(std::string_view s);
IMGUI_API bool InputText(const char *label, std::string *s, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void *user_data = nullptr);
IMGUI_API bool InputText(const char *label, Engine::CoWString *s, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void *user_data = nullptr);

IMGUI_API bool ValueTypeTypePicker(Engine::ValueType *v);

IMGUI_API void BeginTreeArrow(const void *label, ImGuiTreeNodeFlags flags = 0);
IMGUI_API bool EndTreeArrow(bool *opened = nullptr);
IMGUI_API void BeginSpanningTreeNode(const void *id, const char *label, ImGuiTreeNodeFlags flags = 0);
IMGUI_API bool EndSpanningTreeNode();
IMGUI_API bool EditableTreeNode(const void *id, std::string *s, ImGuiTreeNodeFlags flags = 0);

IMGUI_API void Duration(std::chrono::nanoseconds dur);
IMGUI_API void RightAlignDuration(std::chrono::nanoseconds dur);

IMGUI_API void RightAlignText(const char *s, ...);
IMGUI_API void RightAlign(float size);

IMGUI_API bool DragMatrix3(const char *label, Engine::Matrix3 *m, float v_speed);
IMGUI_API bool DragMatrix3(const char *label, Engine::Matrix3 *m, float *v_speeds);
IMGUI_API bool DragMatrix4(const char *label, Engine::Matrix4 *m, float v_speed);
IMGUI_API bool DragMatrix4(const char *label, Engine::Matrix4 *m, float *v_speeds);

IMGUI_API bool MethodPicker(const char *label, const std::vector<std::pair<std::string, Engine::BoundApiFunction>> &methods, Engine::BoundApiFunction *m, std::string *currentName, std::string *filter = nullptr, int expectedArgumentCount = -1);

IMGUI_API void DraggableValueTypeSource(std::string_view name, void (*source)(Engine::ValueType &, void *), void *data, ImGuiDragDropFlags flags = 0);
template <typename T>
void DraggableValueTypeSource(std::string_view name, T &&data, ImGuiDragDropFlags flags = 0)
{
    DraggableValueTypeSource(
        name, [](Engine::ValueType &retVal, void *data) { Engine::to_ValueType(retVal, static_cast<T &&>(*static_cast<std::remove_reference_t<T> *>(data))); }, &data, flags);
}

IMGUI_API const Engine::ValueType *GetValuetypePayload();
IMGUI_API bool AcceptDraggableValueType(const ValueTypePayload **payloadPointer = nullptr);
template <typename T, typename Validator = bool (*)(const T &)>
bool AcceptDraggableValueType(
    T &result, const ValueTypePayload **payloadPointer = nullptr, Validator &&validate = [](const T &t) { return true; })
{
    const Engine::ValueType *payload = GetValuetypePayload();
    if (payload) {
        if (Engine::ValueType_is<T>(*payload)) {
            const T &t = Engine::ValueType_as<T>(*payload);
            if (validate(t) && AcceptDraggableValueType(payloadPointer)) {
                result = t;
                return true;
            } else {
                setPayloadStatus("Payload does not validate");
                return false;
            }
        }

        if constexpr (!Engine::ValueTypePrimitive<T>) {
            if (Engine::ValueType_is<T *>(*payload)) {
                const T *t = Engine::ValueType_as<T *>(*payload);
                if (validate(*t) && AcceptDraggableValueType(payloadPointer)) {
                    result = *t;
                    return true;
                } else {
                    setPayloadStatus("Payload does not validate");
                    return false;
                }
            }
        }

        setPayloadStatus("Payload incompatible with target type: "s + typeid(T).name());
    }
    return false;
}
template <typename T, typename Validator = bool (*)(const T *)>
bool AcceptDraggableValueType(
    T *&result, const ValueTypePayload **payloadPointer = nullptr, Validator &&validate = [](const T *t) { return true; })
{
    const Engine::ValueType *payload = GetValuetypePayload();
    if (payload) {
        if (Engine::ValueType_is<T>(*payload)) {
            T &t = Engine::ValueType_as<T>(*payload);
            if (validate(&t) && AcceptDraggableValueType(payloadPointer)) {
                result = &t;
                return true;
            } else {
                setPayloadStatus("Payload does not validate");
            }
        } else if (Engine::ValueType_is<T *>(*payload)) {
            T *t = Engine::ValueType_as<T *>(*payload);
            if (validate(t) && AcceptDraggableValueType(payloadPointer)) {
                result = t;
                return true;
            } else {
                setPayloadStatus("Payload does not validate");
            }
        } else {
            setPayloadStatus("Payload incompatible with target type: "s + typeid(T).name());
        }
    }
    return false;
}

IMGUI_API bool AcceptDraggableValueType(
    Engine::ValueType &result, Engine::ExtendedValueTypeDesc type, const ValueTypePayload **payloadPointer = nullptr, std::function<bool(const Engine::ValueType &)> validate = [](const Engine::ValueType &t) { return true; });
IMGUI_API bool IsDraggableValueTypeBeingAccepted(const ValueTypePayload **payloadPointer = nullptr);
template <typename T, typename Validator = bool (*)(const T &)>
bool IsDraggableValueTypeBeingAccepted(
    T &result, const ValueTypePayload **payloadPointer = nullptr, Validator &&validate = [](const T &t) { return true; })
{
    const Engine::ValueType *payload = GetValuetypePayload();
    if (payload) {
        if (Engine::ValueType_is<T>(*payload)) {
            const T &t = Engine::ValueType_as<T>(*payload);
            if (validate(t) && IsDraggableValueTypeBeingAccepted(payloadPointer)) {
                result = t;
                return true;
            } else {
                setPayloadStatus("Payload does not validate");
            }
        } else {
            setPayloadStatus("Payload incompatible with target type: "s + typeid(T).name());
        }
    }
    return false;
}

struct FilesystemPickerOptions {
    const char *(*mIconLookup)(const Engine::Filesystem::Path &path, bool isDir) = nullptr;
};

IMGUI_API FilesystemPickerOptions *GetFilesystemPickerOptions();

IMGUI_API bool DirectoryPicker(Engine::Filesystem::Path *path, Engine::Filesystem::Path *selection, bool &accepted, const FilesystemPickerOptions *options = nullptr);
IMGUI_API bool FilePicker(Engine::Filesystem::Path *path, Engine::Filesystem::Path *selection, bool &accepted, bool allowNewFile = false, const FilesystemPickerOptions *options = nullptr);

struct InteractiveViewState {
    bool mMouseDown[3] = { false, false, false };
    bool mDragging[3] = { false, false, false };
    bool mMouseClicked[3] = { false, false, false };
    bool mActive = false;
};

IMGUI_API bool InteractiveView(InteractiveViewState &state);

IMGUI_API void BeginGroupPanel(const char *name, const ImVec2 &size = ImVec2(0.0f, 0.0f));
IMGUI_API void EndGroupPanel();

IMGUI_API bool BeginPopupCompoundContextItem(const char *str_id = nullptr);
IMGUI_API bool BeginPopupCompoundContextWindow(const char *str_id = nullptr, ImGuiPopupFlags popup_flags = 1);

IMGUI_API bool IsNewWindow(const char *name);
IMGUI_API void SetWindowDockingDir(ImGuiID dockSpaceId, ImGuiDir dir, float ratio, bool outer, ImGuiCond cond = 0);

template <typename E>
bool EnumCombo(const char* name, E *val) {
    bool changed = false;
    if (ImGui::BeginCombo(name, std::string { val->toString() }.c_str())) {
        for (E v : E::values()) {
            if (ImGui::Selectable(std::string { v.toString() }.c_str())) {
                *val = v;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

}
