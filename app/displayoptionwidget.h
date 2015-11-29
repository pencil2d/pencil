#ifndef DISPLAYOPTIONDOCKWIDGET_H
#define DISPLAYOPTIONDOCKWIDGET_H

#include "basedockwidget.h"

namespace Ui
{
    class DisplayOption;
}
class Editor;
class QToolButton;


class DisplayOptionWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit DisplayOptionWidget( QWidget* parent );
    virtual ~DisplayOptionWidget();

    void initUI() override;
    void updateUI() override;

    void makeConnectionToEditor(Editor* editor);


public slots:
    void loadUI();


private:
    Ui::DisplayOption* ui = nullptr;

    Editor* mEditor;
};

#endif // DISPLAYOPTIONDOCKWIDGET_H
