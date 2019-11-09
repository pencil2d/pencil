#ifndef KEYFRAMETEXTEDIT_H
#define KEYFRAMETEXTEDIT_H

#include <QPlainTextEdit>

class KeyFrameTextEdit : public QPlainTextEdit {

    Q_OBJECT
public:
    KeyFrameTextEdit(QWidget* parent = nullptr);

Q_SIGNALS:
    void lostFocus();

protected:
    void focusOutEvent(QFocusEvent *e) override;

};

#endif // KEYFRAMETEXTEDIT_H
