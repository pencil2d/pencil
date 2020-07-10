#ifndef ADDTRANSPARENCYTOPAPERDIALOG_H
#define ADDTRANSPARENCYTOPAPERDIALOG_H

#include <QDialog>
#include <QImage>
#include <QGraphicsScene>

class Editor;

namespace Ui {
class AddTransparencyToPaperDialog;
}

class AddTransparencyToPaperDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTransparencyToPaperDialog(QDialog *parent = nullptr);
    ~AddTransparencyToPaperDialog();

    void setCore(Editor* editor);

public slots:
    void updateTraceButton();
    void loadDrawing(int frame);

signals:
    void closeDialog();

private:
    Ui::AddTransparencyToPaperDialog *ui;

    void traceScannedDrawings();
    void closeClicked();

    QGraphicsScene scene;

    QImage mLoadedImage;
    QPixmap mPixmapFromImage;
    Editor* mEditor = nullptr;
};

#endif // ADDTRANSPARENCYTOPAPERDIALOG_H
