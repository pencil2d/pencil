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

#include <QMainWindow>
#include "preferencemanager.h"


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
class ColorInspector;
class RecentFileMenu;
class Timeline2;
class ActionCommands;
class ImportImageSeqDialog;
class BackupElement;



namespace Ui
{
    class MainWindow2;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow2(QWidget* parent = nullptr);
    ~MainWindow2() override;

    Editor* mEditor = nullptr;

    public slots:
    void undoActSetText();
    void undoActSetEnabled();
    void updateSaveState();
    void clearRecentFilesList();

public:
    void setOpacity(int opacity);
    void newDocument(bool force = false);
    void openDocument();
    bool saveDocument();
    bool saveAsNewDocument();
    bool maybeSave();
    bool autoSave();

    // import
    void importImage();
    void importImageSequence();
    void importImageSequenceNumbered();
    void addLayerByFilename(QString strFilePath);
    void importMovie();
    void importGIF();

    void lockWidgets(bool shouldLock);

    void preferences();
    
    void openFile(QString filename);

    PreferencesDialog* getPrefDialog() { return mPrefDialog; }

Q_SIGNALS:
    void updateRecentFilesList(bool b);

protected:
    void tabletEvent(QTabletEvent*) override;
    void closeEvent(QCloseEvent*) override;

private slots:
    void resetAndDockAllSubWidgets();

private:
    bool openObject(QString strFilename, bool checkForChanges);
    bool saveObject(QString strFileName);

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
    void makeConnections(Editor*, ColorBox* colorBox);
    void makeConnections(Editor*, ColorInspector*);
    void makeConnections(Editor*, ScribbleArea*);
    void makeConnections(Editor*, ColorPaletteWidget*);
    void makeConnections(Editor*, TimeLine*);
    void makeConnections(Editor*, DisplayOptionWidget*);
    void makeConnections(Editor*, ToolOptionWidget*);

    void bindActionWithSetting(QAction*, SETTING);

    // UI: Dock widgets
    ColorBox*           mColorBox = nullptr;
    ColorPaletteWidget*   mColorPalette = nullptr;
    DisplayOptionWidget*  mDisplayOptionWidget = nullptr;
    ToolOptionWidget*     mToolOptions = nullptr;
    ToolBoxWidget*        mToolBox = nullptr;
    Timeline2*            mTimeline2 = nullptr;
    RecentFileMenu*       mRecentFileMenu = nullptr;
    PreferencesDialog*    mPrefDialog = nullptr;
    //PreviewWidget*      mPreview = nullptr;
    TimeLine*             mTimeLine = nullptr; // be public temporary
    ColorInspector*       mColorInspector = nullptr;

    // backup
    BackupElement* mBackupAtSave = nullptr;

private:
    ActionCommands* mCommands = nullptr;
    QList< BaseDockWidget* > mDockWidgets;

    QIcon mStartIcon;
    QIcon mStopIcon;

    // a hack for MacOS because closeEvent fires twice
    bool m2ndCloseEvent = false;

    // whether we are currently importing an image sequence.
    bool mIsImportingImageSequence = false;
    
    Ui::MainWindow2* ui = nullptr;
};

#endif // MAINWINDOW2_H
