#ifndef DISPLAYOPTIONDOCKWIDGET_H
#define DISPLAYOPTIONDOCKWIDGET_H

#include <QDockWidget>


class QToolButton;

class DisplayOptionDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    DisplayOptionDockWidget(QWidget *parent = 0);

    QToolButton* thinLinesButton;
    QToolButton* outlinesButton;

    QToolButton* mirrorButton;
    QToolButton* mirrorButtonV;
    QToolButton* onionPrev;
    QToolButton* onionNext;

signals:
    
public slots:

private:
    void createUI();
};

#endif // DISPLAYOPTIONDOCKWIDGET_H
