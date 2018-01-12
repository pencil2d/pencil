/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef TOOLBOXWIDGET_H
#define TOOLBOXWIDGET_H

#include "pencildef.h"
#include "basedockwidget.h"

class QToolButton;
class QGridLayout;
class QIcon;
class SpinSlider;
class DisplayOptionWidget;
class ToolOptionWidget;
class Editor;


class ToolBoxWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    ToolBoxWidget( QWidget* parent );

    void initUI() override;
    void updateUI() override;

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

signals:
    void clearButtonClicked();

private:
    QToolButton* newToolButton(const QIcon&, QString);
    void deselectAllTools();

    QGridLayout* layout;

    QToolButton* pencilButton = nullptr;
    QToolButton* selectButton = nullptr;
    QToolButton* moveButton = nullptr;
    QToolButton* handButton = nullptr;
    QToolButton* eraserButton = nullptr;
    QToolButton* penButton = nullptr;
    QToolButton* polylineButton = nullptr;
    QToolButton* bucketButton = nullptr;
    QToolButton* colouringButton = nullptr;
    QToolButton* eyedropperButton = nullptr;
    QToolButton* smudgeButton = nullptr;
    QToolButton* clearButton = nullptr;

protected:
    void resizeEvent(QResizeEvent* event) override;
};

#endif
