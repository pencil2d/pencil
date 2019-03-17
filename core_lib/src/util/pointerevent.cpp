#include "pointerevent.h"

PointerEvent::PointerEvent(QMouseEvent* event)
{
    mMouseEvent = event;
}

PointerEvent::PointerEvent(QTabletEvent* event)
{
    mTabletEvent = event;
}

PointerEvent::~PointerEvent()
{
}

QPoint PointerEvent::pos() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->pos();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->pos();
    }
    Q_ASSERT(false);
    return QPoint();
}

QPointF PointerEvent::posF() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->localPos();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->posF();
    }
    Q_ASSERT(false);
    return QPointF();
}

Qt::MouseButton PointerEvent::button() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->button();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->button();
    }
    // if we land here... the incoming input was
    // neither tablet nor mouse
    Q_ASSERT(false);
    return Qt::NoButton;
}

Qt::MouseButtons PointerEvent::buttons() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->buttons();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->buttons();
    }
    // if we land here... the incoming input was
    // neither tablet nor mouse
    Q_ASSERT(false);
    return Qt::NoButton;
}

qreal PointerEvent::pressure() const
{
    if (mTabletEvent)
    {
        return mTabletEvent->pressure();
    }
    return 1.0;
}

qreal PointerEvent::rotation() const
{
    if (mTabletEvent)
    {
        return mTabletEvent->rotation();
    }
    return 0.0;
}

qreal PointerEvent::tangentialPressure() const
{
    if (mTabletEvent)
    {
        return mTabletEvent->tangentialPressure();
    }
    return 0.0;
}

int PointerEvent::x() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->x();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->x();
    }
    else
    {
        Q_ASSERT(false);
        return 0;
    }

}

int PointerEvent::y() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->y();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->y();
    }
    else
    {
        Q_ASSERT(false);
        return 0;
    }
}

bool PointerEvent::isTabletEvent() const
{
    if (mTabletEvent)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Qt::KeyboardModifiers PointerEvent::modifiers() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->modifiers();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->modifiers();
    }

    Q_ASSERT(false);
    return Qt::NoModifier;
}

void PointerEvent::accept()
{
    if (mMouseEvent)
    {
        mMouseEvent->accept();
    }
    else if (mTabletEvent)
    {
        mTabletEvent->accept();
    }
    else
    {
        Q_ASSERT(false);
    }
}

void PointerEvent::ignore()
{
    if (mMouseEvent)
    {
        mMouseEvent->ignore();
    }
    else if (mTabletEvent)
    {
        mTabletEvent->ignore();
    }
    else
    {
        Q_ASSERT(false);
    }
}

QEvent::Type PointerEvent::type() const
{
    if (mMouseEvent)
    {
        return mMouseEvent->type();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->type();
    }
    return QEvent::None;
}

QTabletEvent::TabletDevice PointerEvent::device() const
{
    if (mTabletEvent)
    {
        return mTabletEvent->device();
    }
    return QTabletEvent::TabletDevice::NoDevice;
}

QTabletEvent::PointerType PointerEvent::pointerType() const
{
    if (mTabletEvent)
    {
        return mTabletEvent->pointerType();
    }
    return QTabletEvent::PointerType::UnknownPointer;
}

//QEvent::device
