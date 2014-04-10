#ifndef KEYCAPTURELINEEDIT_H
#define KEYCAPTURELINEEDIT_H

#include <QLineEdit>
#include <QKeySequence>

class QKeyEvent;


class KeyCaptureLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit KeyCaptureLineEdit(QWidget* parent = 0);

signals:
    void keyCaptured(QKeySequence seq);

public slots:

protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // KEYCAPTURELINEEDIT_H
