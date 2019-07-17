#pragma once

#include "../tool.h"

#include "widgetsettings.h"

namespace Engine {
namespace Tools {

    class GuiEditor : public Tool<GuiEditor> {
    public:
        GuiEditor(ImGuiRoot &root);

		virtual bool init() override;

        virtual void render() override;
        virtual void renderMenu() override;
        virtual void update() override;

        const char *key() const override;

    private:
        GUI::TopLevelWindow *mWindow;
        WidgetSettings *mSelected = nullptr;
        std::list<WidgetSettings> mSettings;

		bool mMouseDown = false;
		bool mDragging = false;
        bool mDraggingLeft = false, mDraggingTop = false, mDraggingRight = false, mDraggingBottom = false;
    };

}
}