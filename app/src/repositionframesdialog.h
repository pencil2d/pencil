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
    explicit RepositionFramesDialog(QWidget *parent = nullptr);
    ~RepositionFramesDialog();

    void setCore(Editor* editor);

    void setCurrentPolygonF(QPolygonF polygon) { mCurrentPolygonF = polygon; }

public slots:
    void updateDialogText();
    void updateDialogSelectedFrames();
    void repositionFrames();

signals:
    void closeDialog();

private:
    Ui::RepositionFramesDialog *ui;

    void closeClicked();
    QPoint getRepositionPoint();
    int mRepositionFrame;

    QPolygonF mCurrentPolygonF  = QPolygonF();
    QPolygonF mOriginalPolygonF = QPolygonF();
    const QPoint mStartPoint = QPoint(0,0);
    QPoint mEndPoint = QPoint(0,0);
    Editor* mEditor = nullptr;
};

#endif // REPOSITIONFRAMESDIALOG_H
