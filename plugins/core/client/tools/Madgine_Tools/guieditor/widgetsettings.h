#pragma once

#include "Modules/math/matrix3.h"

namespace Engine {
namespace Tools {

    struct WidgetSettings {
        WidgetSettings(GUI::Widget *widget);
        ~WidgetSettings();

        GUI::Widget *widget();

        void render();

        void saveGeometry();
        void applyGeometry();
        void resetGeometry();

		void setSize(const Matrix3 &size);
        void setPos(const Matrix3 &pos);

		std::pair<Matrix3, Matrix3> savedGeometry();

		std::optional<float> aspectRatio();
        void setAspectRatio(std::optional<float> ratio);

		void enforceAspectRatio();

    private:
        GUI::Widget *mWidget;

		Matrix3 mSavedPos, mSavedSize;
		
		bool mEnforceAspectRatio = false;
        float mAspectRatio = 1.0f;
    };

}
}