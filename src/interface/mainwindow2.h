#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QMainWindow>

class QActionGroup;
class Editor;
class Object;
class Palette;
class DisplayOptionDockWidget;
class ToolOptionDockWidget;
template<typename T> class QList;


namespace Ui {
class MainWindow2;
}



class MainWindow2 : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow2(QWidget *parent = 0);
    ~MainWindow2();
    
    void addRecentFile(QString filePath);

    // Data Model
    Object* object;

    // UI: central view
    Editor* editor;

    // UI: Dock widgets
    Palette* m_colorPalette;
    DisplayOptionDockWidget* m_displayOptionWidget;
    ToolOptionDockWidget*    m_toolOptionWidget;

private:
    Ui::MainWindow2 *ui;

    // Old code migration
public slots:
    void setOpacity(int opacity);
    void undoActSetText(void);
    void undoActSetEnabled(void);

private slots:
    void exportFile();
    void toggleLoop(bool);

    void newDocument();
    void openDocument();
    void dockAllPalettes();
    void helpBox();
    void aboutPencil();

private:
    void arrangePalettes();
    void createMenus();
    void loadPlugins();
    void populateMenus(QObject* plugin);
    void addToMenu(QObject* plugin, const QString text, QMenu* menu, const char* member, QActionGroup* actionGroup = 0);

    void closeEvent(QCloseEvent*);
    void keyPressEvent( QKeyEvent* e);
    void keyReleaseEvent( QKeyEvent* e);
    void readSettings();
    void writeSettings();
    //QGraphicsItem *m_svgItem;

    QString path;
    QList<QMenu*>* m_pMenuList;

    //QMenu* fileMenu;
    QMenu* openRecentMenu;
    QMenu* importMenu;
    QMenu* exportMenu;
    QMenu* editMenu;
    QMenu* InsertMenu;
    QMenu* MirrorMenu;
    QMenu* viewMenu;
    QMenu* zoomMenu;
    QMenu* rotateMenu;
    QMenu* layerMenu;
    QMenu* animationMenu;
    QMenu* toolsMenu;
    QMenu* onionskinMenu;
    QMenu* windowsMenu;
    QMenu* helpMenu;


    //QAction* newAct;
    //QAction* openAct;
    //QAction* saveAct;
    //QAction* savAct;
    //QAction* printAct;
    QAction* exportXAct;
    QAction* exportAct;
    //QAction* exitAct;
    QAction* exportimageAct;
    QAction* importMovAct;
    QAction* importMovieAct;

    //QAction* helpMeAct;
    //QAction* aboutPencilAct;

    QAction* trimAct;
    QAction* cropAct;
    QAction* gridAct;
    QAction* onionskinpAct;
    QAction* onionskinnAct;
    QAction* previewAct;
    QAction* rotateAct;
    QAction* rotateAct1;
    QAction* resetpaletteAct;
    QAction* horiMirrorAct;
    QAction* vertiMirrorAct;
    QAction* exportMovAct;
    QAction* exportFlashAct;
    QAction* exportPaletteAct;
    QAction* importPaletteAct;

    QAction* importAct;
    QAction* undoAct;
    QAction* redoAct;
    QAction* copyAct;
    QAction* copyframesAct;
    QAction* cutAct;
    QAction* deleteAct;
    QAction* pasteAct;
    QAction* pasteframesAct;
    QAction* selectAllAct;
    QAction* deselectAllAct;
    QAction* importSndAct;
    QAction* preferencesAct;

    QAction* newBitmapLayerAct;
    QAction* newVectorLayerAct;
    QAction* newSoundLayerAct;
    QAction* newCameraLayerAct;
    QAction* deleteLayerAct;

    QAction* playAnimationAct;
    QAction* loopAnimationAct;
    QAction* extendFrameAct;
    QAction* addFrameAct;
    QAction* duplicateFrameAct;
    QAction* removeFrameAct;
    QAction* nextFrameAct;
    QAction* prevFrameAct;

    QAction* moveToolAct;
    QAction* clearToolAct;
    QAction* selectToolAct;
    QAction* brushToolAct;
    QAction* polylineToolAct;
    QAction* smudgeToolAct;
    QAction* penToolAct;
    QAction* handToolAct;
    QAction* zoomToolAct;
    QAction* zoomAct;
    QAction* zoomAct1;
    QAction* rotatecw;
    QAction* rotateacw;
    QAction* inbetweenAct;
    QAction* inbetweenActV;
    QAction* savesvgAct;

    QAction* pencilToolAct;
    QAction* bucketToolAct;
    QAction* eyedropToolAct;
    QAction* eraserToolAct;
};

#endif // MAINWINDOW2_H
