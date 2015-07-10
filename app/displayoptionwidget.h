#ifndef DISPLAYOPTIONDOCKWIDGET_H
#define DISPLAYOPTIONDOCKWIDGET_H

#include <QDockWidget>

namespace Ui
{
    class DisplayOption;
}
class Editor;
class QToolButton;


class DisplayOptionWidget : public QDockWidget
{
    Q_OBJECT
public:
    DisplayOptionWidget(QWidget *parent = 0);
    void makeConnectionToEditor(Editor* editor);

public slots:
    void changeMultiLayerOnionSkin(bool);
    void multiLayerOnionSkinButtonChanged(bool checked);

private:
    void createUI();

    QToolButton* onionBlueButton;
    QToolButton* onionRedButton;
    QToolButton* gridAButton;
    QToolButton* multiLayerOnionSkinButton;

    Ui::DisplayOption* ui = nullptr;
};

#endif // DISPLAYOPTIONDOCKWIDGET_H
