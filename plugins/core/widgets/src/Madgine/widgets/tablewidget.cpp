#include "../widgetslib.h"

#include "tablewidget.h"

#include "util/layouts/gridlayoutrenderdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN_BASE(Engine::Widgets::TableWidget, Engine::Widgets::WidgetBase)
NAMED_MEMBER(TextData, mTextRenderData)
PROPERTY(RowCount, rowCount, setRowCount)
PROPERTY(ColumnCount, columnCount, setColumnCount)
PROPERTY(ColumnConfigs, columnConfigs, setColumnConfigs)
METATABLE_END(Engine::Widgets::TableWidget)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::TableWidget, Engine::Widgets::WidgetBase)
FIELD(mTextRenderData)
FIELD(mRowCount)
ENCAPSULATED_FIELD(mColumnConfigs, columnConfigs, setColumnConfigs)
SERIALIZETABLE_END(Engine::Widgets::TableWidget)

namespace Engine {
namespace Widgets {

    void TableWidget::setRowCount(uint32_t count)
    {

        if (mRowCount != count) {
            mRowCount = count;
            applyGeometry();

            mCellData.resize(count * columnCount());
        }
    }

    void TableWidget::setColumnCount(uint32_t count)
    {
        size_t oldColumnCount = columnCount();
        if (oldColumnCount != count) {
            mColumnConfigs.resize(count);
            applyGeometry();

            size_t rows = rowCount();

            if (count < oldColumnCount && count > 0) {
                AreaView<std::string, 2> sourceView { mCellData.data(), { oldColumnCount, rows } };
                AreaView<std::string, 2> targetView { mCellData.data(), { count, rows } };
                std::ranges::move(sourceView.subArea({ 0, 0 }, { count, rows }), targetView.begin());
            }

            mCellData.resize(rows * count);

            if (count > oldColumnCount && oldColumnCount > 0) {
                AreaView<std::string, 2> sourceView { mCellData.data(), { oldColumnCount, rows } };
                AreaView<std::string, 2> targetView { mCellData.data(), { count, rows } };
                std::move(sourceView.rbegin(), sourceView.rend(), targetView.subArea({ 0, 0 }, { oldColumnCount, rows }).rbegin());
            }
        }
    }

    uint32_t TableWidget::rowCount() const
    {
        return mRowCount;
    }

    uint32_t TableWidget::columnCount() const
    {
        return mColumnConfigs.size();
    }

    std::vector<ExplicitLayoutConfig> TableWidget::columnConfigs() const
    {
        return mColumnConfigs;
    }

    void TableWidget::setColumnConfigs(const std::vector<ExplicitLayoutConfig> &configs)
    {
        setColumnCount(configs.size());
        mColumnConfigs = configs;
        applyGeometry();
    }

    AreaView<std::string, 2> TableWidget::content()
    {
        return { mCellData.data(), { columnCount(), rowCount() } };
    }

    WidgetClass TableWidget::getClass() const
    {
        return WidgetClass::TABLEWIDGET;
    }

    void TableWidget::vertices(WidgetsRenderData &renderData, size_t layer)
    {
        if (!mTextRenderData.available())
            return;

        Vector3 pos { getAbsolutePosition(), static_cast<float>(depth(layer)) };
        Vector3 size = getAbsoluteSize();

        float fullWidth = mVerticalLayoutRenderData.fullSize();
        float fullHeight = mHorizontalLayoutRenderData.fullSize(mRowCount);

        AreaView<std::string, 2> view { mCellData.data(), { columnCount(), mRowCount } };

        for (size_t row = 0; row < mRowCount; ++row) {
            for (size_t col = 0; col < columnCount(); ++col) {
                auto [cellPos, cellSize] = GridLayoutRenderData::getCellContentDimensions(mHorizontalLayoutRenderData, mVerticalLayoutRenderData, row, col);

                mTextRenderData.render(renderData, view[row][col], pos + cellPos, cellSize);
            }
            if (row + 1 < mRowCount) {
                auto [rowPos, rowSize] = mHorizontalLayoutRenderData.getElementDimensions(row);
                float y = rowPos + rowSize;
                Vector3 leftPoint {
                    pos.x,
                    pos.y + y,
                    pos.z + 0.4f
                };
                Vector3 rightPoint {
                    pos.x + fullWidth,
                    pos.y + y,
                    pos.z + 0.4f
                };
                renderData.mLineData.renderLine(leftPoint, rightPoint);
            }
        }

        if (mColumnConfigs.size() > 0) {
            for (size_t col = 0; col < mColumnConfigs.size() - 1; ++col) {
                auto [colPos, colSize] = mVerticalLayoutRenderData.getElementDimensions(col);
                float x = colPos + colSize;
                Vector3 topPoint {
                    pos.x + x,
                    pos.y,
                    pos.z + 0.4f
                };
                Vector3 bottomPoint {
                    pos.x + x,
                    pos.y + fullHeight,
                    pos.z + 0.4f
                };
                renderData.mLineData.renderLine(topPoint, bottomPoint);
            }
        }
    }

    void TableWidget::sizeChanged(const Vector3i &pixelSize)
    {
        if (mTextRenderData.available()) {
            SizeConstraints rowHeightConstraints = mTextRenderData.calculateHeightConstraints();
            mHorizontalLayoutRenderData.update(mRowCount, rowHeightConstraints, pixelSize.y);
        }
        mVerticalLayoutRenderData.update(mColumnConfigs, pixelSize.x);
    }

}
}
