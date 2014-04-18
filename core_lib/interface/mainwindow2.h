#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QDomElement>
#include <QMainWindow>

class QActionGroup;
class Editor;
class ScribbleArea;
class Object;
class ColorPaletteWidget;
class DisplayOptionDockWidget;
class ToolOptionWidget;
class TimeLine;
class ToolBoxWidget;
class Preferences;
class RecentFileMenu;
template<typename T> class QList;


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

    // Data Model
    Object* m_object;

    // Core controller
    Editor* editor;

    // UI: central Drawing Area
    ScribbleArea* m_pScribbleArea;

    // UI: Dock widgets
    QDockWidget*             m_pColorWheelWidget;
    ColorPaletteWidget*      m_pColorPalette;
    DisplayOptionDockWidget* m_pDisplayOptionWidget;
    ToolOptionWidget*        m_pToolOptionWidget;
    TimeLine*                m_pTimeLine;
    ToolBoxWidget*           m_pToolBox;

    // Other windows
    Preferences* m_pPreferences;

protected:
    void tabletEvent(QTabletEvent* event);
    RecentFileMenu* m_recentFileMenu;

private:
    Ui::MainWindow2* ui;

    // Old code migration
public slots:
    void setOpacity(int opacity);
    void undoActSetText(void);
    void undoActSetEnabled(void);
    void newDocument();
    void openDocument();
    bool saveAsNewDocument();
    void saveDocument();
    bool maybeSave();
    void showPreferences();
    bool openObject(QString strFilename);
    void resetToolsSettings();
    void openFile(QString filename);

private slots:
    bool saveObject(QString strSavedFilename);
    void dockAllPalettes();
    void helpBox();
    void aboutPencil();

    void loadAllShortcuts();
    void unloadAllShortcuts();

    void importPalette();
    void exportPalette();

    // XML save/load
    QDomElement createDomElement(QDomDocument& doc);
    bool loadDomElement(QDomElement docElem, QString filePath);

private:
    void createSubWidgets();
    void makeColorWheelConnections();
    void makeColorPaletteConnections();
    void makeTimeLineConnections();
    void createMenus();

    void closeEvent(QCloseEvent*);

    void readSettings();
    void writeSettings();
};

#endif // MAINWINDOW2_H
