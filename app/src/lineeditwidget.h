#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include <QObject>
#include <QLineEdit>

class Layer;
class QLabel;
class QLineEdit;
class QStackedLayout;

class LineEditWidget : public QLineEdit
{
    Q_OBJECT
public:
    LineEditWidget(QWidget* parent, QString text);

    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void deselect();

private:
    // The stylesheet has to be updated on every event
    // where the read-only property is changed
    void reloadStylesheet();

};

#endif // LINEEDITWIDGET_H
