#include "Meta/metalib.h"

#include "imguiaddons.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "Meta/math/matrix3.h"

#include "Meta/keyvalue/proxyscopebase.h"
#include "Meta/keyvalue/typedscopeptr.h"

#include "Meta/keyvalue/boundapifunction.h"

#include "Interfaces/filesystem/api.h"
#include "Interfaces/filesystem/path.h"

#include "valuetypepayload.h"

Engine::Threading::WorkgroupLocal<ImGuiContext *>
    sContext;

ImGuiContext *&getImGuiContext()
{
    return sContext;
}

namespace ImGui {

ValueTypePayload sPayload;

bool ValueTypeDrawer::draw(Engine::TypedScopePtr &scope)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<scope>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::TypedScopePtr &scope)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<scope>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::OwnedScopePtr &scope)
{
    return draw(static_cast<Engine::TypedScopePtr>(scope));
}

bool ValueTypeDrawer::draw(const Engine::OwnedScopePtr &scope)
{
    return draw(static_cast<Engine::TypedScopePtr>(scope));
}

bool ValueTypeDrawer::draw(bool &b)
{
    return ImGui::Checkbox(mName, &b);
}

bool ValueTypeDrawer::draw(const bool &b)
{
    PushDisabled();
    ImGui::Checkbox(mName, const_cast<bool *>(&b));
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::CoWString &s)
{
    return InputText(mName, &s);
}

bool ValueTypeDrawer::draw(const Engine::CoWString &s)
{
    if (mName) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("\"%.*s\"", static_cast<int>(s.size()), s.data());
    return false;
}

bool ValueTypeDrawer::draw(int &i)
{
    PushItemWidth(100);
    return DragInt(mName, &i);
}

bool ValueTypeDrawer::draw(const int &i)
{
    PushDisabled();
    PushItemWidth(100);
    DragInt(mName, const_cast<int *>(&i));
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(uint64_t &i)
{
    PushItemWidth(100);
    return ImGui::DragScalar(mName, ImGuiDataType_U64, &i, 1.0f);
}

bool ValueTypeDrawer::draw(const uint64_t &i)
{
    PushDisabled();
    PushItemWidth(100);
    ImGui::DragScalar(mName, ImGuiDataType_U64, const_cast<uint64_t *>(&i), 1.0f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(float &f)
{
    PushItemWidth(100);
    return ImGui::DragFloat(mName, &f, 0.15f);
}

bool ValueTypeDrawer::draw(const float &f)
{
    PushDisabled();
    PushItemWidth(100);
    ImGui::DragFloat(mName, const_cast<float *>(&f), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Matrix3 &m)
{
    return ImGui::DragMatrix3(mName, &m, 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Matrix3 &m)
{
    PushDisabled();
    ImGui::DragMatrix3(mName, const_cast<Engine::Matrix3 *>(&m), 0.15f);
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
    return ImGui::DragMatrix4(mName, &m, 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Matrix4 &m)
{
    PushDisabled();
    ImGui::DragMatrix4(mName, const_cast<Engine::Matrix4 *>(&m), 0.15f);
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
    return ImGui::DragFloat2(mName, v.ptr(), 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Vector2 &v)
{
    PushDisabled();
    PushItemWidth(100);
    ImGui::DragFloat2(mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Vector3 &v)
{
    PushItemWidth(100);
    return ImGui::DragFloat3(mName, v.ptr(), 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Vector3 &v)
{
    PushDisabled();
    PushItemWidth(100);
    ImGui::DragFloat3(mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::Vector4 &v)
{
    PushItemWidth(100);
    return ImGui::DragFloat4(mName, v.ptr(), 0.15f);
}

bool ValueTypeDrawer::draw(const Engine::Vector4 &v)
{
    PushDisabled();
    PushItemWidth(100);
    ImGui::DragFloat4(mName, const_cast<float *>(v.ptr()), 0.15f);
    PopDisabled();
    return false;
}

bool ValueTypeDrawer::draw(Engine::KeyValueVirtualRange &it)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<range>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::KeyValueVirtualRange &it)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<range>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::ApiFunction &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<method>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::ApiFunction &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<method>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::BoundApiFunction &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<boundmethod>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::BoundApiFunction &m)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<boundmethod>");
    return false;
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

bool ValueTypeDrawer::draw(const std::monostate &)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<null>");
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
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<object>");
    return false;
}

bool ValueTypeDrawer::draw(const Engine::ObjectPtr &o)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("<object>");
    return false;
}

bool ValueTypeDrawer::draw(Engine::Filesystem::Path &p)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("%s", p.c_str());
    return false;
}

bool ValueTypeDrawer::draw(const Engine::Filesystem::Path &p)
{
    if (strlen(mName)) {
        ImGui::Text("%s: ", mName);
        ImGui::SameLine();
    }
    ImGui::Text("%s", p.c_str());
    return false;
}

void setPayloadStatus(const std::string &msg)
{
    if (ImGui::GetIO().KeyShift)
        sPayload.mStatusMessage = msg;
}

void Text(const std::string &s)
{
    Text("%s", s.c_str());
}

void Text(const std::string_view &s)
{
    Text("%.*s", static_cast<int>(s.size()), s.data());
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

bool InputText(const char *label, Engine::CoWString *s)
{
    char buf[255];
#if WINDOWS
    strncpy_s(buf, sizeof(buf), s->data(), s->size());
#else
    strncpy(buf, s->data(), s->size());
#endif

    if (ImGui::InputText(label, buf, sizeof(buf))) {
        *s = std::string { buf };
        return true;
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
            SelectValueTypeTypes(Engine::type_pack_filter_if_t<Engine::type_pack_apply_t<Engine::type_pack_first_t, Engine::ValueTypeList>, Engine::negate<std::is_default_constructible>::type> {}, v);
            ImGui::EndCombo();
        }
        EndGroup();
        ImGui::PopID();
    }
    return changed;
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

    const char *end = FindRenderedTextEnd(label);
    TextEx(label, end);
    SameLine();

    BeginGroup();
    PushID(label);

    for (int i = 0; i < 3; ++i) {
        PushMultiItemsWidths(3, std::min(300.0f, CalcItemWidth()));
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
        current = m->mScope.name() + ("." + *currentName);
    if (ImGui::BeginCombo(label, current.c_str())) {
        if (filter)
            ImGui::InputText("filter", filter);
        for (auto &[name, method] : methods) {
            if (!filter) {
                if (expectedArgumentCount == -1 || method.argumentsCount() == expectedArgumentCount) {
                    bool is_selected = (method == *m);
                    std::string fullItemName = method.mScope.name() + ("." + name);
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

void DraggableValueTypeSource(const std::string &name, Engine::TypedScopePtr scope, const Engine::ValueType &value, ImGuiDragDropFlags flags)
{
    if (ImGui::BeginDragDropSource(flags)) {
        ValueTypePayload *payload = &sPayload;
        payload->mName = name;
        payload->mSender = scope;
        payload->mValue = value;
        ImGui::SetDragDropPayload("ValueType", &payload, sizeof(payload), ImGuiCond_Once);
        ImGui::Text(name);
        ImGui::Text(value.getTypeString());
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

void RejectDraggableValueType()
{
    ImGui::SetTooltip("Reject");
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

    if (ImGui::BeginChild("CurrentFolder", { 0.0f, -ImGui::GetItemsLineHeightWithSpacing() })) {

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

    if (ImGui::BeginChild("CurrentFolder", { 0.0f, -2 * ImGui::GetItemsLineHeightWithSpacing() })) {

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
}
