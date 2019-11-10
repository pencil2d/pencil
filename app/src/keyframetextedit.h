#ifndef KEYFRAMETEXTEDIT_H
#define KEYFRAMETEXTEDIT_H

#include <QPlainTextEdit>

class KeyFrameTextEdit : public QPlainTextEdit {

    Q_OBJECT
public:
    KeyFrameTextEdit(QWidget* parent = nullptr);

Q_SIGNALS:
    void outsideWidget();

protected:
    void focusOutEvent(QFocusEvent *e) override;
    bool event(QEvent* event) override;

};

#endif // KEYFRAMETEXTEDIT_H
