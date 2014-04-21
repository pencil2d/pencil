#ifndef BASEDOCKWIDGET_H
#define BASEDOCKWIDGET_H

#include <QDockWidget>


class Editor;

class BaseDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit BaseDockWidget(QWidget* pParent);
    ~BaseDockWidget();

    void setCore();

private:
};

#endif // BASEDOCKWIDGET_H
