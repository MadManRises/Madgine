#pragma once

#include "widget.h"

#include "util/layouts/explicitlayoutrenderdata.h"
#include "util/layouts/uniformlayoutrenderdata.h"
#include "util/textrenderdata.h"

#include "Generic/areaview.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT TableWidget : Widget<TableWidget> {

        SERIALIZABLEUNIT(TableWidget);

        using Widget::Widget;
        virtual ~TableWidget() = default;

        void setRowCount(uint32_t count);
        void setColumnCount(uint32_t count);

        uint32_t rowCount() const;
        uint32_t columnCount() const;

        std::vector<ExplicitLayoutConfig> columnConfigs() const;
        void setColumnConfigs(const std::vector<ExplicitLayoutConfig> &configs);

        AreaView<std::string, 2> content();

        virtual WidgetClass getClass() const override;

        virtual void vertices(WidgetsRenderData &renderData, size_t layer) override;

        virtual void sizeChanged(const Vector3i &pixelSize) override;

        TextRenderData mTextRenderData;

    private:
        UniformLayoutRenderData mHorizontalLayoutRenderData;
        ExplicitLayoutRenderData mVerticalLayoutRenderData;

        std::vector<std::string> mCellData;

        uint32_t mRowCount = 0;        
        std::vector<ExplicitLayoutConfig> mColumnConfigs;
    };
}
}

REGISTER_TYPE(Engine::Widgets::TableWidget)