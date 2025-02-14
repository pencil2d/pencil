/*

Pencil2D - Traditional Animation Software
Copyright (C) 2020 David Lamhauge

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef ADDTRANSPARENCYTOPAPERDIALOG_H
#define ADDTRANSPARENCYTOPAPERDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

#include "bitmapimage.h"

class Editor;
class QAbstractButton;
class QGraphicsPixmapItem;

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

    void traceScannedDrawings();

protected:
    void resizeEvent(QResizeEvent*) override;

private slots:
    void SpinboxChanged(int value);
    void SliderChanged(int value);
    void updateDrawing();
    void layerChanged(int index);
    void checkerStateChanged(bool state);
    void zoomChanged(int zoomLevel);
    void buttonClicked(QAbstractButton* button);

private:
    void updatePreview();
    void loadDrawing(int frame);

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
