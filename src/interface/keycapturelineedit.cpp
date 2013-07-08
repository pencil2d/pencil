
#include <QKeyEvent>
#include <QDebug>
#include "keycapturelineedit.h"



KeyCaptureLineEdit::KeyCaptureLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    qDebug("Key Capture!!!!!!!!!!!!");
}

void KeyCaptureLineEdit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control ||
        event->key() == Qt::Key_Shift   ||
        event->key() == Qt::Key_Alt     ||
        event->key() == Qt::Key_Meta )
    {
        // only modifier key is not allowed.
        qDebug() << "only!";
        return;
    }

    int keyInt = event->key();

    if (event->modifiers() & Qt::CTRL)
    {
        keyInt += Qt::Key_Control;
        qDebug() << "Command!";
    }
    if (event->modifiers() & Qt::SHIFT)
    {
        keyInt += Qt::Key_Shift;
    }
    if (event->modifiers() & Qt::ALT)
    {
        keyInt += Qt::Key_Alt;
    }
    if (event->modifiers() & Qt::META)
    {

    }

    QString strKeySeq = QKeySequence(keyInt).toString(QKeySequence::NativeText);

    //m_currentKeySeqItem->setText(strKeySeq);
    //qDebug() << "Current Item:" << m_currentKeySeqItem->text();

    //qDebug() << strKeySeq;
}
