/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef TOOLSET_H
#define TOOLSET_H

#include <QtGui>

class SpinSlider;


class ToolSet : public QWidget
{
    Q_OBJECT

public:
    ToolSet();

    QDockWidget* drawPalette;
    QDockWidget* optionPalette;
    QDockWidget* displayPalette;
    QDockWidget* keyPalette;
    QDockWidget* onionPalette;
    QDockWidget* timePalette;

    QDockWidget* createDisplayPalette();
    QDockWidget* createKeyPalette();
    QDockWidget* createOptionPalette();

public slots:
    void setCounter(int);
    void setWidth(qreal);
    void setFeather(qreal);
    void setOpacity(qreal);
    void setInvisibility(int);
    void setPressure(int);
    void setPreserveAlpha(int);
    void setFollowContour(int);
    void setColour(QColor);
    //void playStart();
    void changePencilButton();
    void changeEraserButton();
    void changeSelectButton();
    void changeMoveButton();
    void changeHandButton();
    void changePenButton();
    void changePolylineButton();
    void changeBucketButton();
    void changeEyedropperButton();
    void changeColouringButton();
    void changeThinLinesButton(bool);
    void changeOutlinesButton(bool);
    void changeSmudgeButton();

    void resetMirror();
    void resetMirrorV();
    void pressureClick(bool);
    void invisibleClick(bool);
    void preserveAlphaClick(bool);
    void followContourClick(bool);

    void onionPrevChanged(bool);
    void onionNextChanged(bool);

signals:
    void rmClick();
    void playClick();
    void addClick();
    void pencilClick();
    void eraserClick();
    void selectClick();
    void moveClick();
    void handClick();
    void penClick();
    void pressureClick(int);
    void invisibleClick(int);
    void preserveAlphaClick(int);
    void followContourClick(int);
    void polylineClick();
    void bucketClick();
    void eyedropperClick();
    void colouringClick();
    void smudgeClick();

    void lengthClick(QString);
    void fpsClick(int);
    void clearClick();
    void thinLinesClick();
    void outlinesClick();
    void mirrorClick();
    void mirrorVClick();
    void togglePrev(bool);
    void toggleNext(bool);
    void widthClick(qreal);
    void featherClick(qreal);
    void opacityClick(qreal);
    void colourClick();
    void loopClick();
    void soundClick();

private:
    void newToolButton(QToolButton*& toolButton);

    QToolButton* choseColour;
    QPixmap* colourSwatch;
    QToolButton* pencilButton;
    QToolButton* selectButton;
    QToolButton* moveButton;
    QToolButton* handButton;
    QToolButton* eraserButton;
    QToolButton* penButton;
    QToolButton* polylineButton;
    QToolButton* bucketButton;
    QToolButton* colouringButton;
    QToolButton* eyedropperButton;
    QToolButton* magnifyButton;
    QToolButton* smudgeButton;

    QToolButton* thinLinesButton;
    QToolButton* outlinesButton;

    QToolButton* mirrorButton;
    QToolButton* mirrorButtonV;
    QToolButton* onionPrev;
    QToolButton* onionNext;
    QToolButton* clearButton;

    QToolButton* add;
    QToolButton* rm;

    QSpinBox* fpsBox;
    //QDoubleSpinBox *sizeSlider;
    SpinSlider* sizeSlider;
    SpinSlider* featherSlider;
    SpinSlider* opacitySlider;
    //QSlider *featherSlider;
    QLabel* framecounter;
    QPushButton* play;

    QCheckBox* usePressureBox;
    QCheckBox* makeInvisibleBox;
    QCheckBox* preserveAlphaBox;
    QCheckBox* followContourBox;

    QCheckBox* onionPrevBox;
    QCheckBox* onionNextBox;
    QCheckBox* loopBox;
    QCheckBox* soundBox;
    int fps;

    void deselectAllTools();
};

#endif
