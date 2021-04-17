#ifndef BUCKETOPTIONSWIDGET_H
#define BUCKETOPTIONSWIDGET_H

#include <QWidget>
#include "pencildef.h"

class Editor;

namespace Ui {
class BucketOptionsWidget;
}

class BucketOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BucketOptionsWidget(Editor* editor, QWidget *parent = nullptr);
    ~BucketOptionsWidget();

    void setColorToleranceEnabled(bool enabled);
    void setFillExpandEnabled(bool enabled);
    void setFillExpand(int fillExpandValue);
    void setColorTolerance(int tolerance);
    void onPropertyChanged(ToolType, const ToolPropertyType propertyType);

private:
    Ui::BucketOptionsWidget *ui;
    Editor* mEditor = nullptr;

    const static int MAX_EXPAND = 25;
    const static int MAX_COLOR_TOLERANCE = 100;
};

#endif // BUCKETOPTIONSWIDGET_H
