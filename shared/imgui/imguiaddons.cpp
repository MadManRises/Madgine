#include "Meta/metalib.h"
#include "Modules/moduleslib.h"

#include "imguiaddons.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"

#include "Meta/math/matrix3.h"

#include "Meta/keyvalue/typedscopeptr.h"

#include "Meta/keyvalue/boundapifunction.h"

#include "Interfaces/filesystem/api.h"
#include "Interfaces/filesystem/path.h"

#include "valuetypepayload.h"

#include "Generic/enum.h"

#include "Modules/threading/workgroupstorage.h"

#include "Generic/coroutines/generator.h"

Engine::Threading::WorkgroupLocal<ImGuiContext *>
    sContext;

ImGuiContext *&getImGuiContext()
{
    return sContext;
}

namespace ImGui {

static ImVector<ImRect> sGroupPanelLabelStack;

static ValueTypePayload sPayload;

void LabeledText(const char *label, const char *text, ...)
{
    int columns = GetColumnsCount();
    assert(columns == 1 || columns == 2);
    va_list args;
    va_start(args, text);

    if (columns == 1) {
        if (strlen(label)) {
            ImGui::LabelTextV(label, text, args);
        } else {
            ImGui::TextV(text, args);
        }
    } else {
        if (strlen(label))
            ImGui::Text("%s", label);
        ImGui::NextColumn();
        ImGui::TextV(text, args);
        ImGui::NextColumn();
    }
    va_end(args);
}

bool ValueTypeDrawer::draw(Engine::TypedScopePtr &scope)
{
    LabeledText(mName, "<scope>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::TypedScopePtr &scope)
{
    LabeledText(mName, "<scope>");
    return false;
}

bool ValueTypeDrawer::draw(bool &b)
{
    return Col(LIFT(Checkbox), mName, &b);
}

bool ValueTypeDrawer::draw(const bool &b)
{
    PushDisabled();
    Col(LIFT(Checkbox), mName, const_cast<bool *>(&b));
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::CoWString &s)
{
    return Col(LIFT(InputText), mName, &s, 0);
}

bool ValueTypeDrawer::draw(const Engine::CoWString &s)
{
    LabeledText(mName, "\"%.*s\"", static_cast<int>(s.size()), s.data());
    return false;
}

bool ValueTypeDrawer::draw(int &i)
{
    PushItemWidth(100);
    return Col(LIFT(DragInt), mName, &i);
}

bool ValueTypeDrawer::draw(const int &i)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragInt), mName, const_cast<int *>(&i));
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(uint64_t &i)
{
    PushItemWidth(100);
    return Col(LIFT(DragScalar), mName, ImGuiDataType_U64, &i, 1.0f);
}

bool ValueTypeDrawer::draw(const uint64_t &i)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragScalar), mName, ImGuiDataType_U64, const_cast<uint64_t *>(&i), 1.0f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(float &f)
{
    PushItemWidth(100);
    return Col(LIFT(DragFloat), mName, &f, 0.15f);
}

bool ValueTypeDrawer::draw(const float &f)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragFloat), mName, const_cast<float *>(&f), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Matrix3 &m)
{
    return Col(LIFT(DragMatrix3), mName, &m, 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Matrix3 &m)
{
    PushDisabled();
    Col(LIFT(DragMatrix3), mName, const_cast<Engine::Matrix3 *>(&m), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Matrix3 *m)
{
    return draw(*m);
}

bool ValueTypeDrawer::draw(const Engine::Matrix3 *m)
{
    return draw(*m);
}

bool ValueTypeDrawer::draw(Engine::Matrix4 &m)
{
    return Col(LIFT(DragMatrix4), mName, &m, 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Matrix4 &m)
{
    PushDisabled();
    Col(LIFT(DragMatrix4), mName, const_cast<Engine::Matrix4 *>(&m), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Matrix4 *m)
{
    return draw(*m);
}

bool ValueTypeDrawer::draw(const Engine::Matrix4 *m)
{
    return draw(*m);
}

bool ValueTypeDrawer::draw(Engine::Vector2 &v)
{
    PushItemWidth(100);
    return Col(LIFT(DragFloat2), mName, v.ptr(), 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Vector2 &v)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragFloat2), mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Vector3 &v)
{
    PushItemWidth(100);
    return Col(LIFT(DragFloat3), mName, v.ptr(), 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Vector3 &v)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragFloat3), mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Vector4 &v)
{
    PushItemWidth(100);
    return Col(LIFT(DragFloat4), mName, v.ptr(), 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Vector4 &v)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragFloat4), mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Vector2i &v)
{
    PushItemWidth(100);
    return Col(LIFT(DragInt2), mName, v.ptr());
}

bool ValueTypeDrawer::draw(const Engine::Vector2i &v)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragInt2), mName, const_cast<int *>(v.ptr()));
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Vector3i &v)
{
    PushItemWidth(100);
    return Col(LIFT(DragInt3), mName, v.ptr());
}

bool ValueTypeDrawer::draw(const Engine::Vector3i &v)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragInt3), mName, const_cast<int *>(v.ptr()));
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Vector4i &v)
{
    PushItemWidth(100);
    return Col(LIFT(DragInt4), mName, v.ptr());
}

bool ValueTypeDrawer::draw(const Engine::Vector4i &v)
{
    PushDisabled();
    PushItemWidth(100);
    Col(LIFT(DragInt4), mName, const_cast<int *>(v.ptr()));
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::KeyValueVirtualSequenceRange &it)
{
    LabeledText(mName, "<range>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::KeyValueVirtualSequenceRange &it)
{
    LabeledText(mName, "<range>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::KeyValueVirtualAssociativeRange &it)
{
    LabeledText(mName, "<map>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::KeyValueVirtualAssociativeRange &it)
{
    LabeledText(mName, "<map>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::KeyValueFunction &m)
{
    LabeledText(mName, "<function>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::KeyValueFunction &m)
{
    LabeledText(mName, "<function>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::ApiFunction &m)
{
    LabeledText(mName, "<api-function>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::ApiFunction &m)
{
    LabeledText(mName, "<api-function>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::BoundApiFunction &m)
{
    LabeledText(mName, "<bound api-function>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::BoundApiFunction &m)
{
    LabeledText(mName, "<bound api-function>");
    return false;
}

bool ValueTypeDrawer::draw(std::monostate &)
{
    LabeledText(mName, "<null>");
    return false;
}

bool ValueTypeDrawer::draw(const std::monostate &)
{
    LabeledText(mName, "<null>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::Quaternion &q)
{
    Engine::Vector3 v = q.toDegrees();

    if (draw(v)) {
        q = Engine::Quaternion::FromDegrees(v);
        return true;
    }
    return false;
}

bool ValueTypeDrawer::draw(const Engine::Quaternion &q)
{
    const Engine::Vector3 v = q.toDegrees();
    return draw(v);
}

bool ValueTypeDrawer::draw(Engine::ObjectPtr &o)
{
    LabeledText(mName, "<object>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::ObjectPtr &o)
{
    LabeledText(mName, "<object>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::Filesystem::Path &p)
{
    LabeledText(mName, p.c_str());
    return false;
}

bool ValueTypeDrawer::draw(const Engine::Filesystem::Path &p)
{
    LabeledText(mName, p.c_str());
    return false;
}

bool ValueTypeDrawer::draw(Engine::EnumHolder &e)
{
    int columns = GetColumnsCount();
    assert(columns == 2 || columns == 0);
    bool changed = false;
    std::string name { e.toString() };
    if (ImGui::BeginCombo(mName, name.c_str())) {
        for (int32_t i : e.table()->values<int32_t>()) {
            bool isSelected = e.value() == i;
            std::string valueName { e.table()->toString(i) };
            if (ImGui::Selectable(valueName.c_str(), isSelected)) {
                e.setValue(i);
                changed = true;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return changed;
}

bool ValueTypeDrawer::draw(const Engine::EnumHolder &e)
{
    std::string_view name = e.toString();
    LabeledText(mName, "\"%.*s\"", static_cast<int>(name.size()), name.data());
    return false;
}

void setPayloadStatus(std::string_view msg)
{
    if (ImGui::GetIO().KeyShift)
        sPayload.mStatusMessage = msg;
}

void Text(std::string_view s)
{
    Text("%.*s", static_cast<int>(s.size()), s.data());
}

bool InputText(const char *label, std::string *s, ImGuiInputTextFlags flags)
{
    char buf[255];
#if WINDOWS
    strcpy_s(buf, sizeof(buf), s->c_str());
#else
    strcpy(buf, s->c_str());
#endif

    bool result = ImGui::InputText(label, buf, sizeof(buf), flags);
    if (result || (flags & ImGuiInputTextFlags_EnterReturnsTrue)) {
        *s = buf;
        return result;
    }
    return false;
}

bool InputText(const char *label, Engine::CoWString *s, ImGuiInputTextFlags flags)
{
    char buf[255];
#if WINDOWS
    strncpy_s(buf, sizeof(buf), s->data(), s->size());
#else
    strncpy(buf, s->data(), s->size());
#endif

    bool result = ImGui::InputText(label, buf, sizeof(buf), flags);

    if (result || (flags & ImGuiInputTextFlags_EnterReturnsTrue)) {
        *s = std::string { buf };
        return result;
    }
    return false;
}

template <typename T>
bool SelectValueTypeType(Engine::ValueType *v)
{
    bool result = Selectable(Engine::toValueTypeIndex<T>().toString().data(), v->is<T>());
    if (result)
        *v = T {};
    return result;
}

template <typename... Ty>
bool SelectValueTypeTypes(Engine::type_pack<Ty...>, Engine::ValueType *v)
{
    return (SelectValueTypeType<Ty>(v) | ...);
}

void BeginValueType(Engine::ExtendedValueTypeDesc type, const char *name)
{
    if (type.mType == Engine::ExtendedValueTypeEnum::GenericType) {
        const float width = CalcItemWidth() - GetFrameHeight();

        ImGui::PushID(name);
        BeginGroup();
        ImGui::PushItemWidth(width);
    }
}

bool EndValueType(Engine::ValueType *v, Engine::ExtendedValueTypeDesc type)
{
    bool changed = false;
    if (type.mType == Engine::ExtendedValueTypeEnum::GenericType) {
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 0.0f);
        if (ImGui::BeginCombo("##combo", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft)) {
            changed = true;
            SelectValueTypeTypes(Engine::type_pack_filter_if_t<Engine::type_pack_apply_t<Engine::type_pack_first_t, Engine::ValueTypeList>, std::is_default_constructible> {}, v);
            ImGui::EndCombo();
        }
        EndGroup();
        ImGui::PopID();
    }
    return changed;
}

void BeginTreeArrow(const void *label, ImGuiTreeNodeFlags flags)
{
    ImGui::PushID(label);
    ImGuiID id = ImGui::GetID("treeArrow");

    ImGuiStorage *storage = ImGui::GetStateStorage();
    bool *opened = storage->GetBoolRef(id);

    ImGuiWindow *window = ImGui::GetCurrentWindow();
    ImGuiContext &g = *GImGui;
    const ImVec2 label_size = ImGui::CalcTextSize("asd", nullptr, false);
    const ImGuiStyle &style = g.Style;
    const float text_base_offset_y = ImMax(0.0f, window->DC.CurrLineTextBaseOffset); // Latch before ItemSize changes it
    const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y);
    ImRect frame_bb = ImRect(window->DC.CursorPos, ImVec2(window->Pos.x + ImGui::GetContentRegionMax().x, window->DC.CursorPos.y + frame_height));
    ItemSize(ImVec2(label_size.x, frame_height), text_base_offset_y);

    if (!(flags & ImGuiTreeNodeFlags_Leaf)) {
        ImGui::RenderArrow(ImVec2(style.FramePadding.x + frame_bb.Min.x, g.FontSize * 0.15f + text_base_offset_y + frame_bb.Min.y), *opened ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);

        const ImRect interact_bb = ImRect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + style.ItemSpacing.x * 2, frame_bb.Max.y);

        bool pressed = ImGui::ButtonBehavior(interact_bb, id, nullptr, nullptr);

        if (pressed)
            *opened = !*opened;
    }
}

bool EndTreeArrow(bool *opened)
{
    ImGuiID id = ImGui::GetID("treeArrow");
    if (!opened) {
        ImGuiStorage *storage = ImGui::GetStateStorage();
        opened = storage->GetBoolRef(id);
    }

    ImGui::PopID();

    if (*opened)
        ImGui::TreePushOverrideID(id);

    return *opened;
}

void BeginSpanningTreeNode(const void *id, const char *label, ImGuiTreeNodeFlags flags)
{

    BeginTreeArrow(id, flags);
    ImGui::SameLine(0.0f, 0.0f);

    bool b = ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAllColumns);

    if (b && !(flags & ImGuiTreeNodeFlags_Leaf)) {
        ImGuiStorage *storage = ImGui::GetStateStorage();
        bool *opened = storage->GetBoolRef(ImGui::GetID("treeArrow"));
        *opened = !*opened;
    }
}

bool EndSpanningTreeNode()
{
    return EndTreeArrow();
}

bool EditableTreeNode(const void *id, std::string *s, ImGuiTreeNodeFlags flags)
{
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;

    BeginTreeArrow(id, flags);
    ImGui::SameLine(0.0f, 0.0f);

    ImVec2 pos_before = window->DC.CursorPos;

    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.FramePadding.y * 2.0f));
    bool b = Selectable("##Selectable", flags & ImGuiTreeNodeFlags_Selected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowItemOverlap);
    PopStyleVar();

    ImGuiID inputId = window->GetID("##Input");
    bool temp_input_is_active = TempInputIsActive(inputId);
    bool temp_input_start = b && IsMouseDoubleClicked(0);

    if (temp_input_start)
        SetActiveID(inputId, window);

    char buf[255];
#if WINDOWS
    strncpy_s(buf, sizeof(buf), s->data(), s->size());
#else
    strncpy(buf, s->data(), s->size());
#endif

    if (temp_input_is_active || temp_input_start) {
        ImVec2 pos_after = window->DC.CursorPos;
        window->DC.CursorPos = pos_before;

        if (TempInputText(window->DC.LastItemRect, inputId, "##Input", buf, sizeof(buf), ImGuiInputTextFlags_None))
            *s = std::string { buf };
        window->DC.CursorPos = pos_after;
    } else {
        window->DrawList->AddText(pos_before, GetColorU32(ImGuiCol_Text), buf);
    }

    return EndTreeArrow();
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

    BeginGroup();
    PushID(label);

    for (int i = 0; i < 3; ++i) {
        PushMultiItemsWidths(3, std::min(300.0f, CalcItemWidth()));
        for (int j = 0; j < 3; ++j) {
            PushID(3 * i + j);
            if (j > 0) {
                SameLine(0, g.Style.ItemInnerSpacing.x);
            }
            if (enabled && !enabled[3 * i + j])
                PushDisabled();
            value_changed |= DragFloat("", &(*m)[i][j], v_speeds[3 * i + j]);
            if (enabled && !enabled[3 * i + j])
                PopDisabled();
            PopItemWidth();
            PopID();
        }
        if (i == 0) {
            SameLine(0, g.Style.ItemInnerSpacing.x);
            const char *end = FindRenderedTextEnd(label);
            TextEx(label, end);
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

bool DragMatrix4(const char *label, Engine::Matrix4 *m, float *v_speeds, bool *enabled)
{
    ImGuiWindow *window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    bool value_changed = false;

    const char *end = FindRenderedTextEnd(label);
    TextEx(label, end);
    SameLine();

    BeginGroup();
    PushID(label);

    for (int i = 0; i < 4; ++i) {
        PushMultiItemsWidths(4, std::min(400.0f, CalcItemWidth()));
        for (int j = 0; j < 4; ++j) {
            PushID(4 * i + j);
            if (enabled && !enabled[4 * i + j])
                PushDisabled();
            value_changed |= DragFloat("", &(*m)[i][j], v_speeds[4 * i + j]);
            if (enabled && !enabled[4 * i + j])
                PopDisabled();
            PopItemWidth();
            PopID();
            if (j < 3) {
                SameLine(0, g.Style.ItemInnerSpacing.x);
            }
        }
    }
    PopID();
    EndGroup();

    return value_changed;
}

bool DragMatrix4(const char *label, Engine::Matrix4 *m, float v_speed, bool *enabled)
{
    float speeds[16];
    std::fill_n(speeds, 16, v_speed);
    return DragMatrix4(label, m, speeds, enabled);
}

bool MethodPicker(const char *label, const std::vector<std::pair<std::string, Engine::BoundApiFunction>> &methods, Engine::BoundApiFunction *m, std::string *currentName, std::string *filter, int expectedArgumentCount)
{
    bool result = false;

    if (!label)
        label = "##testid";

    std::string current;
    if (m->mScope)
        current = m->scope().name() + ("." + *currentName);
    if (ImGui::BeginCombo(label, current.c_str())) {
        if (filter)
            ImGui::InputText("filter", filter);
        for (auto &[name, method] : methods) {
            if (!filter) {
                if (expectedArgumentCount == -1 || method.argumentsCount() == expectedArgumentCount) {
                    bool is_selected = (method == *m);
                    std::string fullItemName = method.scope().name() + ("." + name);
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

void DraggableValueTypeSource(std::string_view name, void (*source)(Engine::ValueType &, void *), void *data, ImGuiDragDropFlags flags)
{
    if (ImGui::BeginDragDropSource(flags)) {
        ValueTypePayload *payload = &sPayload;
        payload->mName = name;
        source(payload->mValue, data);
        ImGui::SetDragDropPayload("ValueType", &payload, sizeof(payload), ImGuiCond_Once);
        ImGui::Text(name);
        ImGui::Text(payload->mValue.getTypeString());
        if (!payload->mStatusMessage.empty()) {
            ImGui::Text(payload->mStatusMessage);
            payload->mStatusMessage.clear();
        }
        ImGui::EndDragDropSource();
    }
}

const ValueTypePayload *GetValuetypePayloadData()
{
    const ImGuiPayload *payload = ImGui::GetDragDropPayload();

    if (payload && payload->IsDataType("ValueType")) {
        return *static_cast<const ValueTypePayload **>(payload->Data);
    }

    return nullptr;
}

const Engine::ValueType *GetValuetypePayload()
{
    const ValueTypePayload *payload = GetValuetypePayloadData();
    if (payload) {
        return &payload->mValue;
    }
    return nullptr;
}

bool AcceptDraggableValueType(
    Engine::ValueType &result, Engine::ExtendedValueTypeDesc type, const ValueTypePayload **payloadPointer, std::function<bool(const Engine::ValueType &)> validate)
{
    const Engine::ValueType *payload = GetValuetypePayload();
    if (payload) {
        if (validate(*payload) && type.canAccept(payload->type()) && AcceptDraggableValueType(payloadPointer)) {
            result = *payload;
            return true;
        }
    }
    return false;
}

bool AcceptDraggableValueType(const ValueTypePayload **payloadPointer)
{
    if (ImGui::AcceptDragDropPayload("ValueType")) {
        if (payloadPointer)
            *payloadPointer = GetValuetypePayloadData();
        return true;
    }
    return false;
}

bool IsDraggableValueTypeBeingAccepted(const ValueTypePayload **payloadPointer)
{
    ImGuiContext &g = *GImGui;
    if (!g.DragDropActive)
        return false;

    ImGuiWindow *window = g.CurrentWindow;
    if (!(window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect))
        return false;
    if (g.HoveredWindowUnderMovingWindow == NULL || window->RootWindow != g.HoveredWindowUnderMovingWindow->RootWindow)
        return false;

    const ImRect &display_rect = (window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HasDisplayRect) ? window->DC.LastItemDisplayRect : window->DC.LastItemRect;
    ImGuiID id = window->DC.LastItemId;
    if (id == 0)
        id = window->GetIDFromRectangle(display_rect);
    if (g.DragDropPayload.SourceId == id)
        return false;

    if (g.DragDropAcceptIdPrev == id) {
        if (payloadPointer)
            *payloadPointer = GetValuetypePayloadData();
        return true;
    }
    return false;
}

void BeginFilesystemPicker(Engine::Filesystem::Path *path, Engine::Filesystem::Path *selection)
{
    if (path->empty())
        *path = Engine::Filesystem::Path { "." }.absolute();
    if (selection->empty())
        *selection = Engine::Filesystem::Path { "." }.absolute();

    if (ImGui::Button("Up")) {
        *selection = *path;
        *path = *path / "..";
    }

    ImGui::SameLine();

    if (ImGui::BeginCombo("Current", path->c_str())) {

        ImGui::EndCombo();
    }
}

bool EndFilesystemPicker(bool valid, bool &accepted, bool openWrite = false, bool askForConfirmation = false, bool alreadyClicked = false)
{
    bool closed = false;

    if (ImGui::Button("Cancel")) {
        accepted = false;
        closed = true;
    }
    ImGui::SameLine();
    if (!valid)
        PushDisabled();
    if (ImGui::Button(openWrite ? "Save" : "Open") || alreadyClicked) {
        if (!askForConfirmation) {
            accepted = true;
            closed = true;
        } else {
            ImGui::OpenPopup("Confirmation");
        }
    }
    if (!valid)
        PopDisabled();

    return closed;
}

bool DirectoryPicker(Engine::Filesystem::Path *path, Engine::Filesystem::Path *selection, bool &accepted)
{
    BeginFilesystemPicker(path, selection);

    if (ImGui::BeginChild("CurrentFolder", { 0.0f, -ImGui::GetFrameHeightWithSpacing() })) {

        for (Engine::Filesystem::FileQueryResult result : Engine::Filesystem::listDirs(*path)) {

            bool selected = *selection == result.path();

            if (ImGui::Selectable(result.path().filename().c_str(), selected)) {
                *selection = result.path();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                *path = result.path();
            }
        }

        ImGui::EndChild();
    }

    return EndFilesystemPicker(true, accepted);
}

bool FilePicker(Engine::Filesystem::Path *path, Engine::Filesystem::Path *selection, bool &accepted, bool openWrite)
{
    BeginFilesystemPicker(path, selection);

    bool selectedIsFile = false;
    bool selectedIsDir = false;
    bool clicked = false;

    if (ImGui::BeginChild("CurrentFolder", { 0.0f, -2 * ImGui::GetFrameHeightWithSpacing() })) {

        for (Engine::Filesystem::FileQueryResult result : Engine::Filesystem::listFilesAndDirs(*path)) {

            bool selected = *selection == result.path();
            if (selected) {
                if (result.isDir())
                    selectedIsDir = true;
                else
                    selectedIsFile = true;
            }

            if (ImGui::Selectable(result.path().filename().c_str(), selected)) {
                *selection = result.path();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                if (result.isDir()) {
                    *path = result.path();
                } else {
                    *selection = result.path();
                    clicked = true;
                }
            }
        }

        ImGui::EndChild();
    }

    bool validPath = true;

    std::string fileName = selection->relative(*path).str();
    if (InputText("Filename:", &fileName)) {
        Engine::Filesystem::Path p = fileName;
        if (!p.empty() && p.isRelative())
            *selection = *path / p;
        else
            validPath = false;
    }

    bool confirmed = false;

    if (openWrite) {
        if (ImGui::BeginPopup("Confirmation")) {
            ImGui::Text("Are you sure you want to overwrite file '%s'?", selection->c_str());
            if (ImGui::Button("Yes")) {
                confirmed = true;
                clicked = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    return EndFilesystemPicker(validPath && (selectedIsFile || (openWrite && !selectedIsDir)), accepted, openWrite, openWrite && selectedIsFile && !confirmed, clicked);
}

bool InteractiveView(InteractiveViewState &state)
{
    state.mActive = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    ImGuiIO &io = ImGui::GetIO();

    if (state.mActive) {
        for (int i = 0; i < 3; ++i) {
            if (io.MouseClicked[i]) {
                state.mMouseDown[i] = true;
            }
        }
    }

    for (int i = 0; i < 3; ++i) {
        state.mMouseClicked[i] = false;
        if (state.mMouseDown[i]) {

            if (Engine::Vector2 { io.MouseDelta }.length() >= io.MouseDragThreshold / 3.0f && !state.mDragging[0] && !state.mDragging[1] && !state.mDragging[2]) {
                state.mDragging[i] = true;
            }

            if (io.MouseReleased[i]) {
                state.mMouseDown[i] = false;
                if (!state.mDragging[i]) {
                    state.mMouseClicked[i] = true;
                }
                state.mDragging[i] = false;
            }
        }
    }

    return state.mActive;
}

void ImGui::BeginGroupPanel(const char *name, const ImVec2 &size)
{
    ImGui::BeginGroup();

    auto itemSpacing = ImGui::GetStyle().ItemSpacing;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();
    ImGui::BeginGroup();

    ImVec2 effectiveSize = size;
    if (size.x < 0.0f)
        effectiveSize.x = ImGui::GetContentRegionAvailWidth();
    else
        effectiveSize.x = size.x;
    ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::TextUnformatted(name);
    auto labelMin = ImGui::GetItemRectMin();
    auto labelMax = ImGui::GetItemRectMax();
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
    ImGui::BeginGroup();

    //ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x -= frameHeight;

    auto itemWidth = ImGui::CalcItemWidth();
    ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

    sGroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
}

void ImGui::EndGroupPanel()
{
    ImGui::PopItemWidth();

    auto itemSpacing = ImGui::GetStyle().ItemSpacing;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();

    ImGui::EndGroup();

    //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

    ImGui::EndGroup();

    auto itemMin = ImGui::GetItemRectMin();
    auto itemMax = ImGui::GetItemRectMax();
    //ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

    auto labelRect = sGroupPanelLabelStack.back();
    sGroupPanelLabelStack.pop_back();

    ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
    ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
    labelRect.Min.x -= itemSpacing.x;
    labelRect.Max.x += itemSpacing.x;
    for (int i = 0; i < 4; ++i) {
        switch (i) {
        // left half-plane
        case 0:
            ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true);
            break;
        // right half-plane
        case 1:
            ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true);
            break;
        // top
        case 2:
            ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true);
            break;
        // bottom
        case 3:
            ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true);
            break;
        }

        ImGui::GetWindowDrawList()->AddRect(
            frameRect.Min, frameRect.Max,
            ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
            halfFrame.x);

        ImGui::PopClipRect();
    }

    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x += frameHeight;

    ImGui::Dummy(ImVec2(0.0f, 0.0f));

    ImGui::EndGroup();
}

bool BeginPopupCompoundContextItem(const char *str_id)
{
    ImGuiWindow *window = GImGui->CurrentWindow;
    if (!str_id)
        str_id = "compound_context";
    ImGuiID id = ImHashStr(str_id, 0, ImHashData(&window, sizeof(window)));

    ImRect rect { GetItemRectMin(), GetItemRectMax() };

    bool open = BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

    if (open) {
        ImVec2 pos = GetMousePosOnOpeningCurrentPopup();
        if (rect.Contains(pos)) {
            ImGui::Separator();
        }else{
            open = false;
            EndPopup();
        }
    }

    return open;
}

bool BeginPopupCompoundContextWindow(const char *str_id, ImGuiPopupFlags popup_flags)
{
    ImGuiWindow *window = GImGui->CurrentWindow;
    if (!str_id)
        str_id = "compound_context";
    ImGuiID id = ImHashStr(str_id, 0, ImHashData(&window, sizeof(window)));
    int mouse_button = (popup_flags & ImGuiPopupFlags_MouseButtonMask_);
    if (IsMouseReleased(mouse_button) && IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        OpenPopupEx(id, popup_flags);
    return BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
}

}
