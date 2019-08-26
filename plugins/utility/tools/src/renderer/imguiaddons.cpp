#include "../toolslib.h"

#include "imguiaddons.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "Modules/math/matrix3.h"

#include "Modules/generic/templates.h"
#include "Modules/keyvalue/typedscopeptr.h"

namespace ImGui {

bool ValueTypeDrawer::draw(Engine::TypedScopePtr &scope)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<scope>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::TypedScopePtr &scope)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<scope>");
}

bool ValueTypeDrawer::draw(Engine::InvScopePtr &p)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<invptr>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::InvScopePtr &p)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<invptr>");
}

bool ValueTypeDrawer::draw(bool &b)
{
    return ImGui::Checkbox(mName, &b);
}

void ValueTypeDrawer::draw(const bool &b)
{
    PushDisabled();
    ImGui::Checkbox(mName, const_cast<bool *>(&b));
    PopDisabled();
}

bool ValueTypeDrawer::draw(std::string &s)
{
    return InputText(mName, &s);
}

void ValueTypeDrawer::draw(const std::string &s)
{
    if (mName) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("\"%s\"", s.c_str());
}

bool ValueTypeDrawer::draw(int &i)
{
    return DragInt(mName, &i);
}

void ValueTypeDrawer::draw(const int &i)
{
    PushDisabled();
    DragInt(mName, const_cast<int *>(&i));
    PopDisabled();
}

bool ValueTypeDrawer::draw(size_t &i)
{
    return ImGui::DragScalar(mName, ImGuiDataType_U32, &i, 1.0f);
}

void ValueTypeDrawer::draw(const size_t &i)
{
    PushDisabled();
    ImGui::DragScalar(mName, ImGuiDataType_U32, const_cast<size_t *>(&i), 1.0f);
    PopDisabled();
}

bool ValueTypeDrawer::draw(float &f)
{
    return ImGui::DragFloat(mName, &f, 0.15f);
}

void ValueTypeDrawer::draw(const float &f)
{
    PushDisabled();
    ImGui::DragFloat(mName, const_cast<float *>(&f), 0.15f);
    PopDisabled();
}

bool ValueTypeDrawer::draw(Engine::Matrix3 &m)
{
    return ImGui::DragMatrix3(mName, &m, 0.15f);
}

void ValueTypeDrawer::draw(const Engine::Matrix3 &m)
{
    PushDisabled();
    ImGui::DragMatrix3(mName, const_cast<Engine::Matrix3 *>(&m), 0.15f);
    PopDisabled();
}

bool ValueTypeDrawer::draw(Engine::Vector2 &v)
{
    return ImGui::DragFloat2(mName, v.ptr(), 0.15f);
}

void ValueTypeDrawer::draw(const Engine::Vector2 &v)
{
    PushDisabled();
    ImGui::DragFloat2(mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
}

bool ValueTypeDrawer::draw(Engine::Vector3 &v)
{
    return ImGui::DragFloat3(mName, v.ptr(), 0.15f);
}

void ValueTypeDrawer::draw(const Engine::Vector3 &v)
{
    PushDisabled();
    ImGui::DragFloat3(mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
}

bool ValueTypeDrawer::draw(Engine::Vector4 &v)
{
    return ImGui::DragFloat4(mName, v.ptr(), 0.15f);
}

void ValueTypeDrawer::draw(const Engine::Vector4 &v)
{
    PushDisabled();
    ImGui::DragFloat4(mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
}

bool ValueTypeDrawer::draw(Engine::KeyValueVirtualIterator &it)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<iterator>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::KeyValueVirtualIterator &it)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<iterator>");
}

bool ValueTypeDrawer::draw(Engine::ApiMethod &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<method>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::ApiMethod &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<method>");
}

bool ValueTypeDrawer::draw(Engine::BoundApiMethod &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<boundmethod>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::BoundApiMethod &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<boundmethod>");
}

bool ValueTypeDrawer::draw(std::monostate &)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<null>");
    return false;
}

void ValueTypeDrawer::draw(const std::monostate &)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<null>");
}

bool ValueTypeDrawer::draw(Engine::Scripting::LuaTable &t)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<table>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::Scripting::LuaTable &t)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<table>");
}

bool ValueTypeDrawer::draw(Engine::Quaternion &q)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<quaternion>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::Quaternion &q)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<quaternion>");
}

bool ValueTypeDrawer::draw(Engine::ObjectPtr &o)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<object>");
    return false;
}

void ValueTypeDrawer::draw(const Engine::ObjectPtr& o) 
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<object>");
}

bool InputText(const char *label, std::string *s)
{
    char buf[255];
#if WINDOWS
    strcpy_s(buf, sizeof(buf), s->c_str());
#else
    strcpy(buf, s->c_str());
#endif

    if (ImGui::InputText(label, buf, sizeof(buf))) {
        *s = buf;
        return true;
    }
    return false;
}

template <typename T>
bool SelectValueTypeType(Engine::ValueType *v)
{
    bool result = Selectable(Engine::ValueType::getTypeString(static_cast<Engine::ValueType::Type>(Engine::variant_index<Engine::ValueType::Union, T>::value)).c_str(), v->is<T>());
    if (result)
        *v = T {};
    return result;
}

template <typename... T>
bool SelectValueTypeTypes(std::variant<T...>, Engine::ValueType *v)
{
    return (SelectValueTypeType<T>(v) | ...);
}

bool ValueType(Engine::ValueType *v, bool allowTypeSwitch, const char *name)
{
    if (allowTypeSwitch) {
        const float width = CalcItemWidth() - GetFrameHeight();

        ImGui::PushID(name);
        BeginGroup();
        ImGui::PushItemWidth(width);
    }

    bool result = v->visit([&](auto &tmp) {
        return ValueTypeDrawer { name }.draw(tmp);
    });

    if (allowTypeSwitch) {
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 0.0f);
        if (ImGui::BeginCombo("##combo", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft)) {
            SelectValueTypeTypes(Engine::ValueType::Union {}, v);
            ImGui::EndCombo();
        }
        EndGroup();
        ImGui::PopID();
    }
    return result;
}

void PushDisabled()
{
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
}

void PopDisabled()
{
    ImGui::PopStyleVar();
    ImGui::PopItemFlag();
}

bool SpanningTreeNode(const void *id, const char *label, bool leaf)
{
    ImGui::PushID(id);
    ImGuiStorage *storage = ImGui::GetStateStorage();
    bool *opened = storage->GetBoolRef(ImGui::GetID(id));
    if (!leaf) {
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        ImGuiContext &g = *GImGui;
        const ImVec2 label_size = ImGui::CalcTextSize("asd", nullptr, false);
        const ImGuiStyle &style = g.Style;
        const float text_base_offset_y = ImMax(0.0f, window->DC.CurrLineTextBaseOffset); // Latch before ItemSize changes it
        const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y);
        ImRect frame_bb = ImRect(window->DC.CursorPos, ImVec2(window->Pos.x + ImGui::GetContentRegionMax().x, window->DC.CursorPos.y + frame_height));
        ImGui::RenderArrow(ImVec2(style.FramePadding.x + frame_bb.Min.x, g.FontSize * 0.15f + text_base_offset_y + frame_bb.Min.y), *opened ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);
    }
    ImGui::Indent(20.0f);
    bool b = ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAllColumns);
    ImGui::Unindent(20.0f);
    if (!leaf && b) {
        *opened = !*opened;
    }
    ImGui::PopID();
    return *opened;
}

void Duration(std::chrono::nanoseconds dur)
{
    if (dur.count() < 1000) {
        ImGui::Text("%lld ns", dur.count());
    } else if (dur.count() < 1000000) {
        ImGui::Text("%.3f us", std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(dur).count());
    } else if (dur.count() < 1000000000) {
        ImGui::Text("%.4f ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(dur).count());
    } else {
        ImGui::Text("%.4f  s", std::chrono::duration_cast<std::chrono::duration<float>>(dur).count());
    }
}

void RightAlignDuration(std::chrono::nanoseconds dur)
{
    if (dur.count() < 1000) {
        ImGui::RightAlignText("%lld ns", dur.count());
    } else if (dur.count() < 1000000) {
        ImGui::RightAlignText("%.3f us", std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(dur).count());
    } else if (dur.count() < 1000000000) {
        ImGui::RightAlignText("%.4f ms", std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(dur).count());
    } else {
        ImGui::RightAlignText("%.4f  s", std::chrono::duration_cast<std::chrono::duration<float>>(dur).count());
    }
}

void RightAlignText(const char *s, ...)
{
    char buffer[1024];
    va_list args;

    va_start(args, s);
    int len = vsprintf(buffer, s, args);
    va_end(args);
    assert(len >= 0);

    ImGuiContext &g = *ImGui::GetCurrentContext();

    ImFont *font = g.Font;
    const float font_size = g.FontSize;

    float w = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, buffer, buffer + len, NULL).x;

    ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - w);
    ImGui::Text("%s", buffer);
}

bool DragMatrix3(const char *label, Engine::Matrix3 *m, float *v_speeds, bool *enabled)
{
    ImGuiWindow *window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    bool value_changed = false;

    Text("%s", label);
    SameLine();

    BeginGroup();
    PushID(m);

    for (int i = 0; i < 3; ++i) {
        PushMultiItemsWidths(3, CalcItemWidth());
        for (int j = 0; j < 3; ++j) {
            PushID(3 * i + j);
            if (enabled && !enabled[3 * i + j])
                PushDisabled();
            value_changed |= DragFloat("", &(*m)[i][j], v_speeds[3 * i + j]);
            if (enabled && !enabled[3 * i + j])
                PopDisabled();
            PopItemWidth();
            PopID();
            if (j < 2) {
                SameLine(0, g.Style.ItemInnerSpacing.x);
            }
        }
    }
    PopID();
    EndGroup();

    return value_changed;
}

bool DragMatrix3(const char *label, Engine::Matrix3 *m, float v_speed, bool *enabled)
{
    float speeds[9];
    std::fill_n(speeds, 9, v_speed);
    return DragMatrix3(label, m, speeds, enabled);
}

bool MethodPicker(const char *label, const std::vector<std::pair<std::string, Engine::BoundApiMethod>> &methods, Engine::BoundApiMethod *m, std::string *currentName, std::string *filter, int expectedArgumentCount)
{
    bool result = false;

    if (!label)
        label = "##testid";

    std::string current;
    if (m->scope())
        current = m->scope().mType->mName + ("." + *currentName);
    if (ImGui::BeginCombo(label, current.c_str())) {
        if (filter)
            ImGui::InputText("filter", filter);
        for (auto &[name, method] : methods) {
            if (!filter) {
                if (expectedArgumentCount == -1 || method.argumentsCount() == expectedArgumentCount) {
                    bool is_selected = (method == *m);
                    std::string fullItemName = method.scope().mType->mName + ("." + name);
                    if (ImGui::Selectable(fullItemName.c_str(), is_selected)) {
                        *currentName = name;
                        *m = method;
                        result = true;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
            }
        }
        ImGui::EndCombo();
    }
    return result;
}

void DraggableValueTypeSource(const std::string &name, Engine::TypedScopePtr scope, const Engine::ValueType &value)
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ValueTypePayload payload { name, scope, value };
        ImGui::SetDragDropPayload("ValueType", &payload, sizeof(payload), ImGuiCond_Once);
        ImGui::Text("%s", name.c_str());
        ImGui::EndDragDropSource();
    }
}

const ValueTypePayload *GetValuetypePayload()
{
    const ImGuiPayload *payload = ImGui::GetDragDropPayload();

    if (payload && payload->IsDataType("ValueType")) {
        return static_cast<const ValueTypePayload *>(payload->Data);
    }

    return nullptr;
}

bool AcceptingDraggableValueTypeTarget(const ValueTypePayload **payloadPointer)
{
    bool result = false;
    if (ImGui::BeginDragDropTarget()) {
        if (ImGui::AcceptDragDropPayload("ValueType")) {
            if (payloadPointer)
                *payloadPointer = GetValuetypePayload();
            result = true;
        }
        ImGui::EndDragDropTarget();
    }
    return result;
}

void RejectingDraggableValueTypeTarget()
{
    if (ImGui::BeginDragDropTarget()) {
        ImGui::SetTooltip("Reject");
        ImGui::EndDragDropTarget();
    }
}

}