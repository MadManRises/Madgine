#pragma once

#include "toolscollector.h"

#include "widgetsettings.h"

#include "toolbase.h"

namespace Engine {
namespace Tools {

    struct GuiEditor : public Tool<GuiEditor> {

        SERIALIZABLEUNIT(GuiEditor);

        GuiEditor(ImRoot &root);

        virtual Threading::Task<bool> init() override;

        virtual void render() override;
        virtual void renderMenu() override;
        virtual void update() override;

        std::string_view key() const override;

    private:
        void renderSelection(Widgets::WidgetBase *hoveredWidget = nullptr);
        void renderHierarchy(Widgets::WidgetBase **hoveredWidget = nullptr);
        bool drawWidget(Widgets::WidgetBase *w, Widgets::WidgetBase **hoveredWidget = nullptr);

    private:        
        Widgets::WidgetManager *mWidgetManager = nullptr;
        WidgetSettings *mSelected = nullptr;
        std::list<WidgetSettings> mSettings;

        bool mMouseDown = false;
        bool mDragging = false;
        bool mDraggingLeft = false, mDraggingTop = false, mDraggingRight = false, mDraggingBottom = false;
    };

}
}

RegisterType(Engine::Tools::GuiEditor);