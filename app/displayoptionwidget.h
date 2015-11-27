#ifndef DISPLAYOPTIONDOCKWIDGET_H
#define DISPLAYOPTIONDOCKWIDGET_H

#include <QDockWidget>
#include "pencildef.h"
#include "editor.h"
#include "scribblearea.h"
#include "preferencemanager.h"

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
    void loadUI();
    void updateUI();


private:
    Ui::DisplayOption* ui = nullptr;

    Editor* mEditor;
};

#endif // DISPLAYOPTIONDOCKWIDGET_H
