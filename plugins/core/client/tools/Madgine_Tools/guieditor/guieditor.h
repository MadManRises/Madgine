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

		void saveLayout();
        void loadLayout();

    private:
        void renderSelection(GUI::WidgetBase *hoveredWidget = nullptr);
        void renderHierarchy(GUI::WidgetBase **hoveredWidget = nullptr);
        bool drawWidget(GUI::WidgetBase *w, GUI::WidgetBase **hoveredWidget = nullptr);

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