#ifndef DISPLAYOPTIONDOCKWIDGET_H
#define DISPLAYOPTIONDOCKWIDGET_H

#include <QDockWidget>

class Editor;
class QToolButton;

class DisplayOptionDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    DisplayOptionDockWidget(QWidget *parent = 0);
    void makeConnectionToEditor(Editor* editor);

    QToolButton* thinLinesButton;
    QToolButton* outlinesButton;
    QToolButton* mirrorButton;
    QToolButton* mirrorButtonV;
    QToolButton* onionPrevButton;
    QToolButton* onionNextButton;
    QToolButton* onionBlueButton;
    QToolButton* onionRedButton;
    QToolButton* gridAButton;
    QToolButton* gridBButton;
    QToolButton* multiLayerOnionSkinButton;
    QToolButton* onionBlueNextButton;
    QToolButton* onionRedNextButton;

signals:
    
public slots:
    void changeThinLinesButton(bool bIsChecked);
    void changeOutlinesButton(bool bIsChecked);
    void onionPrevChanged(bool checked);
    void onionNextChanged(bool checked);

    void changeMultiLayerOnionSkin(bool);
    void multiLayerOnionSkinButtonChanged(bool checked);

private:
    void createUI();
};

#endif // DISPLAYOPTIONDOCKWIDGET_H
