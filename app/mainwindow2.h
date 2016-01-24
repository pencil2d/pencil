/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2011-2015 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QDomElement>
#include <QMainWindow>
#include "backgroundwidget.h"

template<typename T> class QList;
class QActionGroup;
class Object;
class Editor;
class ScribbleArea;
class BaseDockWidget;
class ColorPaletteWidget;
class DisplayOptionWidget;
class ToolOptionWidget;
class TimeLine;
class ToolBoxWidget;
class PreferencesDialog;
class PreviewWidget;
class ColorBox;
class RecentFileMenu;
class Timeline2;
class CommandCenter;

namespace Ui
{
class MainWindow2;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow2(QWidget* parent = 0);
    ~MainWindow2();

    Editor* mEditor = nullptr;

public:
    void setOpacity(int opacity);
    void undoActSetText(void);
    void undoActSetEnabled(void);
    void newDocument();
    void openDocument();
    void saveDocument();
    bool saveAsNewDocument();
    bool maybeSave();

    // import/export
    void importImage();
    void exportImage();

    void importImageSequence();
    void exportImageSequence();

    void importMovie();
    void exportMovie();

    void preferences();
    void helpBox();
    void aboutPencil();

    void openFile(QString filename);

protected:
    void tabletEvent( QTabletEvent* ) override;
    void closeEvent( QCloseEvent* ) override;

private:

    bool openObject( QString strFilename );
    bool saveObject( QString strFileName );

    void dockAllSubWidgets();

    void createDockWidgets();
    void createMenus();
    void setMenuActionChecked( QAction*, bool bChecked );
    void setupKeyboardShortcuts();
    void clearKeyboardShortcuts();

    void importPalette();
    void exportPalette();

    void readSettings();
    void writeSettings();

    void makeConnections( Editor*, ColorBox* );
    void makeConnections( Editor*, ScribbleArea* );
    void makeConnections( Editor*, ColorPaletteWidget* );
    void makeConnections( Editor*, TimeLine* );
    void makeConnections( Editor*, DisplayOptionWidget* );
    void makeConnections( Editor*, ToolOptionWidget*);

    // UI: central Drawing Area
    ScribbleArea* mScribbleArea;

    // UI: Dock widgets
    ColorBox*             mColorWheel          = nullptr;
    ColorPaletteWidget*   mColorPalette        = nullptr;
    DisplayOptionWidget*  mDisplayOptionWidget = nullptr;
    ToolOptionWidget*     mToolOptions         = nullptr;
    ToolBoxWidget*        mToolBox             = nullptr;
    Timeline2*            mTimeline2           = nullptr;
    RecentFileMenu*       mRecentFileMenu      = nullptr;
    //PreviewWidget*      mPreview = nullptr;

public:
    TimeLine*             mTimeLine; // be public temporary

private:
    CommandCenter* mCommands              = nullptr;

    Ui::MainWindow2* ui                   = nullptr;
    QList< BaseDockWidget* > mDockWidgets;
    BackgroundWidget* mBackground;
};

#endif // MAINWINDOW2_H
