#ifndef ADDTRANSPARENCYTOPAPERDIALOG_H
#define ADDTRANSPARENCYTOPAPERDIALOG_H

#include <QDialog>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "bitmapimage.h"

class Editor;

namespace Ui {
class AddTransparencyToPaperDialog;
}

class AddTransparencyToPaperDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTransparencyToPaperDialog(QDialog *parent = nullptr);
    ~AddTransparencyToPaperDialog() override;

    void setCore(Editor* editor);

    void initUI();

signals:
    void closeDialog();

protected:
    void resizeEvent(QResizeEvent*) override;

private slots:
    void SpinboxChanged(int value);
    void SliderChanged(int value);
    void loadDrawing(int frame);
    void updateDrawing();
    void layerChanged(int index);
    void checkerStateChanged(bool state);
    void zoomChanged(int zoomLevel);

private:

    void updatePreview();
    void traceScannedDrawings();
    void closeClicked();

    int mZoomLevel = 1;

    Ui::AddTransparencyToPaperDialog *ui;

    QGraphicsScene scene;
    QGraphicsPixmapItem* mPreviewImageItem = nullptr;

    int mThreshold = 220;
    BitmapImage mBitmap;
    QPixmap mPixmapFromImage;
    Editor* mEditor = nullptr;
};

#endif // ADDTRANSPARENCYTOPAPERDIALOG_H
