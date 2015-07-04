#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QDomElement>
#include <QMainWindow>

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

    Editor* mEditor;

public slots:
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

    void dockAllPalettes();

    void createDockWidgets();
    void createMenus();
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
    QDockWidget*          mColorWheel          = nullptr;
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
    PreferencesDialog* mPreferencesDialog;
    CommandCenter* mCommands;

    Ui::MainWindow2* ui;
    QList< BaseDockWidget* > mDockWidgets;
};

#endif // MAINWINDOW2_H
