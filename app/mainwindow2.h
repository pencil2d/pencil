/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2011-2015 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

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
class ActionCommands;
class ImportImageSeqDialog;


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

    public slots:
    void undoActSetText();
    void undoActSetEnabled();
    void updateSaveState();
    void clearRecentFilesList();

public:
    void setOpacity(int opacity);
    void newDocument();
    void openDocument();
    bool saveDocument();
    bool saveAsNewDocument();
    bool maybeSave();
    bool autoSave();

    // import
    void importImage();
    void importImageSequence();
    void importMovie();

    void lockWidgets(bool shouldLock);

    void preferences();
    
    void openFile(QString filename);

    PreferencesDialog* getPrefDialog() { return mPrefDialog; }

Q_SIGNALS:
    void updateRecentFilesList(bool b);

protected:
    void tabletEvent(QTabletEvent*) override;
    void closeEvent(QCloseEvent*) override;

private:
    bool openObject(QString strFilename);
    bool saveObject(QString strFileName);

    void dockAllSubWidgets();

    void createDockWidgets();
    void createMenus();
    void setMenuActionChecked(QAction*, bool bChecked);
    void setupKeyboardShortcuts();
    void clearKeyboardShortcuts();
    void updateZoomLabel();

    void importPalette();
    void exportPalette();

    void readSettings();
    void writeSettings();

    void changePlayState(bool isPlaying);

    void makeConnections(Editor*);
    void makeConnections(Editor*, ColorBox*);
    void makeConnections(Editor*, ScribbleArea*);
    void makeConnections(Editor*, ColorPaletteWidget*);
    void makeConnections(Editor*, TimeLine*);
    void makeConnections(Editor*, DisplayOptionWidget*);
    void makeConnections(Editor*, ToolOptionWidget*);

    void bindActionWithSetting(QAction*, SETTING);

    // UI: central Drawing Area
    ScribbleArea* mScribbleArea = nullptr;

    // UI: Dock widgets
    ColorBox*             mColorWheel = nullptr;
    ColorPaletteWidget*   mColorPalette = nullptr;
    DisplayOptionWidget*  mDisplayOptionWidget = nullptr;
    ToolOptionWidget*     mToolOptions = nullptr;
    ToolBoxWidget*        mToolBox = nullptr;
    Timeline2*            mTimeline2 = nullptr;
    RecentFileMenu*       mRecentFileMenu = nullptr;
    PreferencesDialog*    mPrefDialog = nullptr;
    //PreviewWidget*      mPreview = nullptr;
    TimeLine*             mTimeLine; // be public temporary

    // backup
    BackupElement* mBackupAtSave = nullptr;

private:
    ActionCommands* mCommands = nullptr;
    QList< BaseDockWidget* > mDockWidgets;
    BackgroundWidget* mBackground = nullptr;;

    QIcon mStartIcon;
    QIcon mStopIcon;

    Ui::MainWindow2* ui = nullptr;
};

#endif // MAINWINDOW2_H
