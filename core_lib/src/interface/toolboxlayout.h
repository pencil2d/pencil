#ifndef TOOLBOXLAYOUT_H
#define TOOLBOXLAYOUT_H

#include "flowlayout.h"

class ToolBoxLayout : public FlowLayout
{
public:
    ToolBoxLayout(QWidget* parent, int margin, int hSpacing, int vSpacing);

protected:
    void lastLineAlignment(int startIndex, int count, RowLayoutInfo rowInfo, const QRect& effectiveRect) const override;

private:
    void alignRowFromRowInfo(int startIndex, int count, RowLayoutInfo rowInfo) const;
};

#endif // TOOLBOXLAYOUT_H
