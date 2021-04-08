/*

Pencil2D - Traditional Animation Software
Copyright (C) 2020 Jakob Gahde

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

#include "pencildef.h"

class Editor;
class QComboBox;
class QLabel;
class QSlider;

/**
 * The status bar of Pencil2D's main window.
 */
class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    /**
     * Constructs a new status bar. For the status bar to work properly, you must also use setEditor() to pass an Editor instance to it.
     *
     * @param parent The parent object of the status bar
     */
    explicit StatusBar(QWidget *parent = nullptr);

    /**
     * Associates an Editor instance with the status bar.
     *
     * This is necessary for most functionality to work.
     *
     * @param editor
     */
    void setEditor(Editor *editor) { mEditor = editor; }

public slots:
    /**
     * Updates the status bar with information about the current tool.
     *
     * @param tool The currently active tool
     */
    void updateToolStatus(ToolType tool);

    /**
     * Updates the file modification status.
     *
     * @param modified Whether the current file contains unsaved modifications
     */
    void updateModifiedStatus(bool modified);

    /**
     * Updates the zoom level displayed in the status bar.
     */
    void updateZoomStatus();

signals:

    /**
     * This signal is sent when the user chooses a new zoom level through the status bar.
     *
     * @param scale The new zoom level selected by the user, represented as a scale factor
     */
    void zoomChanged(double scale);

private:
    /** The editor associated with this status bar */
    Editor *mEditor = nullptr;

    /** Label used to display the icon of the current tool */
    QLabel *mToolIcon = nullptr;
    /** Label used to display a short help text for the current tool */
    QLabel *mToolLabel = nullptr;
    /** Label indicating that the current file contains unsaved changes */
    QLabel *mModifiedLabel = nullptr;
    /** Combo box for choosing pre-defined or custom zoom levels */
    QComboBox *mZoomBox = nullptr;
    /** Slider for adjusting the zoom level */
    QSlider *mZoomSlider = nullptr;
};

#endif // STATUSBAR_H
