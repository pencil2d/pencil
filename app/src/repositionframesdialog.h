#ifndef REPOSITIONFRAMESDIALOG_H
#define REPOSITIONFRAMESDIALOG_H

#include <QDialog>
#include <QPolygonF>

#include "editor.h"

class ToolManager;

namespace Ui {
class RepositionFramesDialog;
}

class RepositionFramesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RepositionFramesDialog(QWidget* parent);
    ~RepositionFramesDialog();

    void setCore(Editor* editor);
    void initUI();

    void setCurrentPolygonF(QPolygonF polygon) { mCurrentPolygonF = polygon; }

public slots:
    void updateDialogText();
    void updateDialogSelectedFrames();
    void repositionFrames();
    void updateRadioButtons();
    void checkboxStateChanged(int i);
    void updateLayersBox();

signals:
    void closeDialog();

private:
    Ui::RepositionFramesDialog *ui;

    void closeClicked();
    void updateLayersToSelect();
    QPoint getRepositionPoint();
    void prepareRepositionSelectedImages(int repositionFrame);

    int mRepositionFrame = 0;
    QList<int> mLayerIndexes;

    QPolygonF mCurrentPolygonF;
    QPolygonF mOriginalPolygonF;
    QPoint mStartPoint = QPoint(0,0);
    QPoint mEndPoint = QPoint(0,0);
    Editor* mEditor = nullptr;
};

#endif // REPOSITIONFRAMESDIALOG_H
