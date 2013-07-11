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
#include "pencildef.h"

class SpinSlider;
class DisplayOptionDockWidget;
class ToolOptionDockWidget;
class Editor;


class ToolSet : public QWidget
{
    Q_OBJECT

public:
    ToolSet(Editor* editor);

    QDockWidget* drawPalette;

    QDockWidget* createOptionPalette();

public slots:
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

    void setCurrentTool(ToolType);

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
    void clearButtonClicked();

private:
    void newToolButton(QToolButton*& toolButton);
    void deselectAllTools();

    Editor* m_pEditor;

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
};

#endif
