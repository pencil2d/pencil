#ifndef KEYCAPTURELINEEDIT_H
#define KEYCAPTURELINEEDIT_H

#include <QLineEdit>


class QKeyEvent;


class KeyCaptureLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit KeyCaptureLineEdit(QWidget *parent = 0);
    
signals:
    
public slots:
    
protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // KEYCAPTURELINEEDIT_H
