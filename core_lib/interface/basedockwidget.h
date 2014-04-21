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

    void setCore( Editor* pCore ) { m_pCore = pCore; }

private:
    Editor* m_pCore;
};

#endif // BASEDOCKWIDGET_H
