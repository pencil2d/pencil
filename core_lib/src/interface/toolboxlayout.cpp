#include "toolboxlayout.h"

ToolBoxLayout::ToolBoxLayout(QWidget* parent, int margin, int hSpacing, int vSpacing)
    : FlowLayout(parent, margin, hSpacing, vSpacing)
{

}

void ToolBoxLayout::lastLineAlignment(int startIndex, int count, RowLayoutInfo rowInfo, const QRect &effectiveRect) const
{
    alignRowFromRowInfo(startIndex, count, rowInfo);
}

void ToolBoxLayout::alignRowFromRowInfo(int startIndex, int count, RowLayoutInfo rowInfo) const
{
    int x = rowInfo.startX;

    for (int i = startIndex; i < startIndex + count; i += 1) {

        if (i > itemList.length() - 1) {
            break;
        }
        QLayoutItem *item = itemList.at(i);
        QSize size = item->geometry().size();
        item->setGeometry(QRect(QPoint(x, item->geometry().y()), size));
        x += size.width() + rowInfo.spacing;
    }
}
