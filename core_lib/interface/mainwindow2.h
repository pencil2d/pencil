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
class Preferences;
class RecentFileMenu;


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

    // XML save/load
    //QDomElement createDomElement(QDomDocument& doc);
    //bool loadDomElement(QDomElement docElem, QString filePath);
    void readSettings();
    void writeSettings();

    void makeColorWheelConnections();
    void makeConnections( Editor*, ScribbleArea* );
    void makeConnections( Editor*, ColorPaletteWidget* );
    void makeConnections( Editor*, TimeLine* );
    void makeConnections( Editor*, DisplayOptionWidget* );

    // UI: central Drawing Area
    ScribbleArea* mScribbleArea;

    // UI: Dock widgets
    QDockWidget*             mColorWheel;
    ColorPaletteWidget*      mColorPalette;
    DisplayOptionWidget*     mDisplayOptionWidget;
    ToolOptionWidget*        mToolOptions;
    ToolBoxWidget*           mToolBox;

    RecentFileMenu* mRecentFileMenu;

public:
    TimeLine*                mTimeLine; // be public temporary

private:
    Preferences* m_pPreferences;

    Ui::MainWindow2* ui;
    QList< BaseDockWidget* > m_subWidgets;
};

#endif // MAINWINDOW2_H
