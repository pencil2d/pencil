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
#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QMainWindow>

template<typename T> class QList;
class QActionGroup;
class Object;
class Editor;
class ScribbleArea;
class BaseDockWidget;
class ColorPaletteWidget;
class DisplayOptionWidget;
class OnionSkinWidget;
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
class QLabel;
class PegBarAlignmentDialog;
enum class SETTING;


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
    void openPegAlignDialog();
    void currentLayerChanged();
    void selectionChanged();
    void viewFlipped();

public:
    void newDocument();
    void openDocument();
    bool saveDocument();
    bool saveAsNewDocument();
    bool maybeSave();
    bool autoSave();
    void emptyDocumentWhenErrorOccurred();

    // import
    void importImage();
    void importImageSequence();
    void importPredefinedImageSet();
    void importLayers();
    void importMovieVideo();
    void importGIF();

    void lockWidgets(bool shouldLock);

    void setOpacity(int opacity);
    void preferences();

    void openFile(const QString& filename);

    PreferencesDialog* getPrefDialog() { return mPrefDialog; }

    void displayMessageBox(const QString& title, const QString& body);
    void displayMessageBoxNoTitle(const QString& body);

signals:
    void updateRecentFilesList(bool b);

protected:
    void tabletEvent(QTabletEvent*) override;
    void closeEvent(QCloseEvent*) override;
    void showEvent(QShowEvent*) override;

private:
    void newObject() const;
    bool newObjectFromPresets(int presetIndex);
    bool openObject(const QString& strFilename);
    bool saveObject(QString strFileName);

    void createDockWidgets();
    void createMenus();
    void setupKeyboardShortcuts();
    void clearKeyboardShortcuts();
    void updateZoomLabel();
    bool loadMostRecent();
    bool tryLoadPreset();

    void openPalette();
    void importPalette();
    void exportPalette();

    void readSettings();
    void writeSettings();
    void resetAndDockAllSubWidgets();

    void changePlayState(bool isPlaying);

    void makeConnections(Editor*);
    void makeConnections(Editor*, ColorBox* colorBox);
    void makeConnections(Editor*, ColorInspector*);
    void makeConnections(Editor*, ScribbleArea*);
    void makeConnections(Editor*, ColorPaletteWidget*);
    void makeConnections(Editor*, TimeLine*);
    void makeConnections(Editor*, DisplayOptionWidget*);
    void makeConnections(Editor*, ToolOptionWidget*);
    void makeConnections(Editor*, OnionSkinWidget*);

    bool tryRecoverUnsavedProject();
    void startProjectRecovery(int result);

    // UI: Dock widgets
    ColorBox*             mColorBox = nullptr;
    ColorPaletteWidget*   mColorPalette = nullptr;
    DisplayOptionWidget*  mDisplayOptionWidget = nullptr;
    ToolOptionWidget*     mToolOptions = nullptr;
    ToolBoxWidget*        mToolBox = nullptr;
    //Timeline2*          mTimeline2 = nullptr;
    RecentFileMenu*       mRecentFileMenu = nullptr;
    PreferencesDialog*    mPrefDialog = nullptr;
    //PreviewWidget*      mPreview = nullptr;
    TimeLine*             mTimeLine = nullptr;
    ColorInspector*       mColorInspector = nullptr;
    OnionSkinWidget*      mOnionSkinWidget = nullptr;

    // backup
    BackupElement* mBackupAtSave = nullptr;

    PegBarAlignmentDialog* mPegAlign = nullptr;

    ActionCommands* mCommands = nullptr;
    QList<BaseDockWidget*> mDockWidgets;

    QIcon mStartIcon;
    QIcon mStopIcon;

    // a hack for MacOS because closeEvent fires twice
    bool m2ndCloseEvent = false;

    // statusbar widgets
    QLabel* mZoomLabel = nullptr;

    // Whether to suppress the auto save dialog due to internal work
    bool mSuppressAutoSaveDialog = false;

    Ui::MainWindow2* ui = nullptr;
};

#endif // MAINWINDOW2_H
