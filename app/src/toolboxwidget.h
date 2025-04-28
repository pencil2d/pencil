#ifndef TOOLBOXWIDGET_H
#define TOOLBOXWIDGET_H

#include <QObject>
#include <QToolButton>
#include <QWidget>

#include "flowlayout.h"
#include "pencildef.h"
#include "toolboxlayout.h"

class Editor;

namespace Ui {
class ToolBoxWidget;
}

class ToolBoxWidget : public QWidget
{
    Q_OBJECT
public:
    ToolBoxWidget(QWidget* parent = nullptr);
    ~ToolBoxWidget() override;

    void setEditor(Editor* editor) { mEditor = editor; }
    void initUI();
    void updateUI();

public slots:
    void setToolChecked(ToolType toolType);
    void onLayerDidChange(int index);
    void pencilOn();
    void eraserOn();
    void selectOn();
    void moveOn();
    void penOn();
    void handOn();
    void polylineOn();
    void bucketOn();
    void eyedropperOn();
    void brushOn();
    void smudgeOn();

protected:
    int getMinHeightForWidth(int width) const;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateLayoutAlignment();
    void deselectAllTools();
    void toolOn(ToolType toolType, QToolButton* toolButton);

    FlowLayout* mFlowlayout = nullptr;

    Ui::ToolBoxWidget* ui = nullptr;
    Editor* mEditor = nullptr;
};

#endif // TOOLBOXWIDGET_H
