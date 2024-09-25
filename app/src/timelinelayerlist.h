/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef TIMELINELAYERLIST_H
#define TIMELINELAYERLIST_H

#include <QMap>
#include <QWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include "timelinelayercell.h"

enum class LayerVisibility;
enum class SETTING;

class Layer;
class TimeLine;
class Editor;
class PreferenceManager;
class TimeLineLayerHeaderWidget;

class QScrollArea;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class QListWidget;
class TimeLineLayerCellGutterWidget;

class TimeLineLayerList : public QWidget
{
    Q_OBJECT

public:
    TimeLineLayerList( TimeLine* parent, Editor* editor);
    ~TimeLineLayerList() override;

    int getLayerHeight() const { return mLayerHeight; }

    int getLayerGutterYPosition(int posY) const;

    void loadLayerCells();
    void onCellDragged(const DragEvent& event, TimeLineLayerCellEditorWidget* editorWidget, int, int y);

signals:
    void cellDraggedY(const DragEvent& event, int y);
    void offsetChanged(int);

public slots:
    void updateContent();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void loadSetting(SETTING setting);

private:

    int getLayerNumber(int y) const;
    int getLayerCellY(int layerNumber) const;

    int getDragToLayerNumber(int y, int gutterPositionY) const;

    void drawContent();

    TimeLine* mTimeLine = nullptr;
    Editor* mEditor = nullptr; // the editor for which this timeLine operates
    PreferenceManager* mPrefs = nullptr;

    QPixmap mPixmapCache;

    int mGutterPositionY = -1;
    int mFromLayer = 0;

    bool mRedrawContent = false;
    int mLayerHeight = 20;

    QMap<int, TimeLineLayerCell*> mLayerCells;
    TimeLineLayerCellGutterWidget* mGutterWidget = nullptr;
};

#endif // TIMELINELAYERLIST_H
