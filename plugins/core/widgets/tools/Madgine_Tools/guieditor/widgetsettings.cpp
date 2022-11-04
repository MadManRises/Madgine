#include "../widgetstoolslib.h"

#include "widgetsettings.h"

#include "imgui/imgui.h"

#include "Madgine/widgets/widget.h"

#include "imgui/imguiaddons.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "imgui/imgui_internal.h"

namespace Engine {
namespace Tools {

    WidgetSettings::WidgetSettings(Widgets::WidgetBase *widget, Inspector &inspector)
        : mWidget(widget)
        , mInspector(inspector)
    {
    }

    WidgetSettings::~WidgetSettings()
    {
    }

    Widgets::WidgetBase *WidgetSettings::widget()
    {
        return mWidget;
    }

    void WidgetSettings::render()
    {

        Matrix3 pos = mWidget->getPos();
        Matrix3 size = mWidget->getSize();

        bool enabled[9] = {
            true, true, true,
            !mEnforceAspectRatio, !mEnforceAspectRatio, !mEnforceAspectRatio,
            true, true, true
        };

        float speeds[9] = {
            0.01f,
            0.01f,
            1.0f,
            0.01f,
            0.01f,
            1.0f,
            0.01f,
            0.01f,
            1.0f,
        };

        ImGui::TableNextRow();  
        ImGui::TableNextColumn();
        ImGui::Text("Pos");
        ImGui::TableNextColumn();
        if (ImGui::DragMatrix3("##Pos", &pos, speeds))
            mWidget->setPos(pos);
        ImGui::TableNextRow();   
        ImGui::TableNextColumn();
        ImGui::Text("Size");
        ImGui::TableNextColumn();
        if (ImGui::DragMatrix3("##Size", &size, speeds, enabled)) {
            mWidget->setSize(size);
            enforceAspectRatio();
        }

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("AspectRatio");
        ImGui::TableNextColumn();
        if (ImGui::Checkbox("##aspectrationEnabled", &mEnforceAspectRatio))
            enforceAspectRatio();
        ImGui::SameLine();

        if (!mEnforceAspectRatio)
            ImGui::BeginDisabled();
        if (ImGui::DragFloat("##aspectratio", &mAspectRatio, 0.05f))
            enforceAspectRatio();
        if (!mEnforceAspectRatio)
            ImGui::EndDisabled();

        mInspector.drawMembers(mWidget, { "Pos", "Size", "Children" });
    }

    void WidgetSettings::saveGeometry()
    {
        mSavedPos = mWidget->getPos();
        mSavedSize = mWidget->getSize();
    }

    void WidgetSettings::applyGeometry()
    {
    }

    void WidgetSettings::resetGeometry()
    {
        mWidget->setPos(mSavedPos);
        mWidget->setSize(mSavedSize);
    }

    void WidgetSettings::setSize(const Matrix3 &size)
    {
        mWidget->setSize(size);
        enforceAspectRatio();
    }

    void WidgetSettings::setPos(const Matrix3 &pos)
    {
        mWidget->setPos(pos);
    }

    std::pair<Matrix3, Matrix3> WidgetSettings::savedGeometry()
    {
        return { mSavedPos, mSavedSize };
    }

    std::optional<float> WidgetSettings::aspectRatio()
    {
        return mEnforceAspectRatio ? mAspectRatio : std::optional<float> {};
    }

    void WidgetSettings::setAspectRatio(std::optional<float> ratio)
    {
        mEnforceAspectRatio = ratio.has_value();
        if (mEnforceAspectRatio)
            mAspectRatio = *ratio;
    }

    void WidgetSettings::enforceAspectRatio()
    {
        if (mEnforceAspectRatio) {
            Matrix3 size = mWidget->getSize();
            for (int i = 0; i < 3; ++i)
                size[1][i] = size[0][i] / mAspectRatio;
            mWidget->setSize(size);
        }
    }

}
}