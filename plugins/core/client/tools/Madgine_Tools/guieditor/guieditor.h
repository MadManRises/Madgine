#pragma once

#include "toolscollector.h"

#include "widgetsettings.h"

namespace Engine {
namespace Tools {

    struct GuiEditor : public Tool<GuiEditor> {    

        SERIALIZABLEUNIT;

        GuiEditor(ImRoot &root);

        virtual bool init() override;

        virtual void render() override;
        virtual void renderMenu() override;
        virtual void update() override;

        const char *key() const override;

    private:
        void renderSelection(GUI::Widget *hoveredWidget = nullptr);
        void renderHierarchy(GUI::Widget **hoveredWidget = nullptr);
        void listWidgets(GUI::Widget *w, GUI::Widget **hoveredWidget = nullptr);

    private:
        GUI::TopLevelWindow &mWindow;
        WidgetSettings *mSelected = nullptr;
        std::list<WidgetSettings> mSettings;

		bool mHierarchyVisible = false;

        bool mMouseDown = false;
        bool mDragging = false;
        bool mDraggingLeft = false, mDraggingTop = false, mDraggingRight = false, mDraggingBottom = false;
    };

}
}