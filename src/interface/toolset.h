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
class DisplayOptionDockWidget;
class ToolOptionDockWidget;

class ToolSet : public QWidget
{
    Q_OBJECT

public:
    ToolSet();

    QDockWidget* drawPalette;
    ToolOptionDockWidget* optionPalette;
    QDockWidget* onionPalette;

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
    void changeSmudgeButton();

signals:
    void pencilClick();
    void eraserClick();
    void selectClick();
    void moveClick();
    void handClick();
    void penClick();
    void polylineClick();
    void bucketClick();
    void eyedropperClick();
    void colouringClick();
    void smudgeClick();
    void clearClick();

    void widthClick(qreal);
    void featherClick(qreal);
    void opacityClick(qreal);
    void colourClick();

private:
    void newToolButton(QToolButton*& toolButton);

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
    QToolButton* clearButton;

    void deselectAllTools();
};

#endif
