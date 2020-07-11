#ifndef ADDTRANSPARENCYTOPAPERDIALOG_H
#define ADDTRANSPARENCYTOPAPERDIALOG_H

#include <QDialog>
#include <QImage>
#include <QGraphicsScene>

class Editor;
class BitmapImage;

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
    void SpinboxChanged(int value);
    void SliderChanged(int value);
    void loadDrawing(int frame);
    void updateDrawing();
    void layerChanged(int index);

signals:
    void closeDialog();

private:
    Ui::AddTransparencyToPaperDialog *ui;

    void traceScannedDrawings();
    void closeClicked();

    QGraphicsScene scene;

    int mThreshold = 220;
    BitmapImage* mBitmap = nullptr;
    QImage mLoadedImage;
    QPixmap mPixmapFromImage;
    Editor* mEditor = nullptr;
};

#endif // ADDTRANSPARENCYTOPAPERDIALOG_H
