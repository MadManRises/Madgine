#pragma once

#include "Modules/keyvalue/typedscopeptr.h"
#include "Modules/keyvalue/valuetype.h"

namespace ImGui {

struct ValueTypeDrawer {
    bool draw(Engine::TypedScopePtr &scope);
    void draw(const Engine::TypedScopePtr &scope);
    bool draw(Engine::InvScopePtr &p);
    void draw(const Engine::InvScopePtr &p);
    bool draw(bool &b);
    void draw(const bool &b);
    bool draw(std::string &s);
    void draw(const std::string &s);
    bool draw(int &i);
    void draw(const int &i);
    bool draw(size_t &i);
    void draw(const size_t &i);
    bool draw(float &f);
    void draw(const float &f);
    bool draw(Engine::Matrix3 &m);
    void draw(const Engine::Matrix3 &m);
    bool draw(Engine::Vector2 &v);
    void draw(const Engine::Vector2 &v);
    bool draw(Engine::Vector3 &v);
    void draw(const Engine::Vector3 &v);
    bool draw(Engine::Vector4 &v);
    void draw(const Engine::Vector4 &v);
    bool draw(Engine::KeyValueVirtualIterator &it);
    void draw(const Engine::KeyValueVirtualIterator &it);
    bool draw(Engine::ApiMethod &m);
    void draw(const Engine::ApiMethod &m);
    bool draw(Engine::BoundApiMethod &m);
    void draw(const Engine::BoundApiMethod &m);
    bool draw(std::monostate &);
    void draw(const std::monostate &);
    bool draw(Engine::Scripting::LuaTable &t);
    void draw(const Engine::Scripting::LuaTable &t);
    bool draw(Engine::Quaternion &q);
    void draw(const Engine::Quaternion &q);

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

bool InputText(const char *label, std::string *s);

bool ValueType(Engine::ValueType *v, bool allowTypeSwitch = false, const char *name = "");

void PushDisabled();
void PopDisabled();

bool SpanningTreeNode(const void *id, const char *label, bool leaf = false);

void Duration(std::chrono::nanoseconds dur);
void RightAlignDuration(std::chrono::nanoseconds dur);

void RightAlignText(const char *s, ...);

bool DragMatrix3(const char *label, Engine::Matrix3 *m, float v_speed, bool *enabled = nullptr);

bool MethodPicker(const char *label, const std::vector<std::pair<std::string, Engine::BoundApiMethod>> &methods, Engine::BoundApiMethod *m, std::string *currentName, std::string *filter = nullptr, int expectedArgumentCount = -1);

void DraggableValueTypeSource(const std::string &name, Engine::TypedScopePtr scope, const Engine::ValueType &value);
const ValueTypePayload *GetValuetypePayload();
bool AcceptingDraggableValueTypeTarget(const ValueTypePayload **payloadPointer = nullptr);
void RejectingDraggableValueTypeTarget();
template <typename T>
bool DraggableValueTypeTarget(T &result, const ValueTypePayload **payloadPointer = nullptr)
{
    const ValueTypePayload *payload = GetValuetypePayload();
    if (payload) {
        if (payload->mValue.is<T>()) {
            if (AcceptingDraggableValueTypeTarget(payloadPointer)) {
                result = payload->mValue.as<T>();
                return true;
            }
        } else {
            RejectingDraggableValueTypeTarget();
        }
    }
    return false;
}

}