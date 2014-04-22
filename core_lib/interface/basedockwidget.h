#ifndef BASEDOCKWIDGET_H
#define BASEDOCKWIDGET_H

#include <QDockWidget>


class Editor;

class BaseDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit BaseDockWidget(QWidget* pParent);
    explicit BaseDockWidget(QWidget* pParent, Qt::WindowFlags );
    ~BaseDockWidget();

    virtual void initUI() = 0;

    Editor* core() { return m_pCore; }
    void setCore( Editor* pCore ) { m_pCore = pCore; }

    
private:
    Editor* m_pCore;
};

#endif // BASEDOCKWIDGET_H
