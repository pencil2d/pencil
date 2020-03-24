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

    QPolygonF mCurrentPolygonF  = QPolygonF();
    QPolygonF mOriginalPolygonF = QPolygonF();
    Editor* mEditor = nullptr;
};

#endif // REPOSITIONFRAMESDIALOG_H
