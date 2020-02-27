#pragma once

#include "imconfig.h"

#include "Modules/keyvalue/typedscopeptr.h"
#include "Modules/keyvalue/valuetype.h"

namespace ImGui {

    typedef int ImGuiDragDropFlags;
    typedef int ImGuiTreeNodeFlags;

struct IMGUI_API ValueTypeDrawer {
    bool draw(Engine::TypedScopePtr &scope);
    bool draw(const Engine::TypedScopePtr &scope);
    bool draw(bool &b);
    bool draw(const bool &b);
    bool draw(std::string &s);
    bool draw(const std::string &s);
    bool draw(std::string_view &s);
    bool draw(const std::string_view &s);
    bool draw(int &i);
    bool draw(const int &i);
    bool draw(size_t &i);
    bool draw(const size_t &i);
    bool draw(float &f);
    bool draw(const float &f);
    bool draw(Engine::Matrix3 &m);
    bool draw(const Engine::Matrix3 &m);
    bool draw(Engine::Matrix3 *m);
    bool draw(const Engine::Matrix3 *m);
    bool draw(Engine::Matrix4 &m);
    bool draw(const Engine::Matrix4 &m);
    bool draw(Engine::Matrix4 *m);
    bool draw(const Engine::Matrix4 *m);
    bool draw(Engine::Vector2 &v);
    bool draw(const Engine::Vector2 &v);
    bool draw(Engine::Vector3 &v);
    bool draw(const Engine::Vector3 &v);
    bool draw(Engine::Vector4 &v);
    bool draw(const Engine::Vector4 &v);
    bool draw(Engine::KeyValueVirtualIterator &it);
    bool draw(const Engine::KeyValueVirtualIterator &it);
    bool draw(Engine::ApiMethod &m);
    bool draw(const Engine::ApiMethod &m);
    bool draw(Engine::BoundApiMethod &m);
    bool draw(const Engine::BoundApiMethod &m);
    bool draw(std::monostate &);
    bool draw(const std::monostate &);
    bool draw(Engine::Quaternion &q);
    bool draw(const Engine::Quaternion &q);
    bool draw(Engine::ObjectPtr &o);
    bool draw(const Engine::ObjectPtr &o);

    const char *mName;
};

struct ValueTypePayload {
    std::string mName;
    Engine::TypedScopePtr mSender;
    Engine::ValueType mValue;
};

namespace details {
    template <typename F, typename T>
    using F_supports = decltype(std::declval<F>()(std::declval<T>()));
}

template <typename F>
bool ValueType(Engine::ValueType *v, F &&f, const char *name = "")
{

    return v->visit([&](auto &tmp) {
        if constexpr (Engine::can_apply<details::F_supports, F, decltype(tmp)>::value) {
            return f(std::forward<decltype(tmp)>(tmp));
        } else {
            return ValueTypeDrawer { name }.draw(tmp);
        }
    });
}

IMGUI_API bool InputText(const char *label, std::string *s);

IMGUI_API bool ValueType(Engine::ValueType *v, bool allowTypeSwitch = false, const char *name = "");

IMGUI_API void PushDisabled();
IMGUI_API void PopDisabled();

IMGUI_API void BeginTreeArrow(const void *label, ImGuiTreeNodeFlags flags = 0);
IMGUI_API bool EndTreeArrow(bool *opened = nullptr);
IMGUI_API void BeginSpanningTreeNode(const void *id, const char *label, ImGuiTreeNodeFlags flags = 0);
IMGUI_API bool EndSpanningTreeNode();

IMGUI_API void Duration(std::chrono::nanoseconds dur);
IMGUI_API void RightAlignDuration(std::chrono::nanoseconds dur);

IMGUI_API void RightAlignText(const char *s, ...);

IMGUI_API bool DragMatrix3(const char *label, Engine::Matrix3 *m, float v_speed, bool *enabled = nullptr);
IMGUI_API bool DragMatrix3(const char *label, Engine::Matrix3 *m, float *v_speeds, bool *enabled = nullptr);
IMGUI_API bool DragMatrix4(const char *label, Engine::Matrix4 *m, float v_speed, bool *enabled = nullptr);
IMGUI_API bool DragMatrix4(const char *label, Engine::Matrix4 *m, float *v_speeds, bool *enabled = nullptr);

IMGUI_API bool MethodPicker(const char *label, const std::vector<std::pair<std::string, Engine::BoundApiMethod>> &methods, Engine::BoundApiMethod *m, std::string *currentName, std::string *filter = nullptr, int expectedArgumentCount = -1);

IMGUI_API void DraggableValueTypeSource(const std::string &name, Engine::TypedScopePtr scope, const Engine::ValueType &value, ImGuiDragDropFlags flags = 0);
IMGUI_API const ValueTypePayload *GetValuetypePayload();
IMGUI_API bool AcceptDraggableValueType(const ValueTypePayload **payloadPointer = nullptr);
template <typename T, typename Validator = bool(*)(const T&)>
bool AcceptDraggableValueType(
    T &result, const ValueTypePayload **payloadPointer = nullptr, Validator &&validate = [](const T &t) { return true; })
{
    const ValueTypePayload *payload = GetValuetypePayload();
    if (payload) {
        if (payload->mValue.is<T>()) {
            if (validate(payload->mValue.as<T>()) && AcceptDraggableValueType(payloadPointer)) {
                result = payload->mValue.as<T>();
                return true;
            }
        } 
    }
    return false;
}
IMGUI_API bool IsDraggableValueTypeBeingAccepted(const ValueTypePayload **payloadPointer = nullptr);
template <typename T, typename Validator = bool (*)(const T &)>
bool IsDraggableValueTypeBeingAccepted(
    T &result, const ValueTypePayload **payloadPointer = nullptr, Validator &&validate = [](const T &t) { return true; })
{
    const ValueTypePayload *payload = GetValuetypePayload();
    if (payload) {
        if (payload->mValue.is<T>()) {
            if (validate(payload->mValue.as<T>()) && IsDraggableValueTypeBeingAccepted(payloadPointer)) {
                result = payload->mValue.as<T>();
                return true;
            }
        } 
    }
    return false;
}

}
