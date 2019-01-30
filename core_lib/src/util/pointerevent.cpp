#include "pointerevent.h"

PointerEvent::PointerEvent(QMouseEvent* event)
{
    this->mouseEvent = event;
}

PointerEvent::PointerEvent(QTabletEvent* event)
{
    this->tabletEvent = event;
    mWidgetPos = event->posF();
}

PointerEvent::~PointerEvent()
{
}

QPoint PointerEvent::pos() const
{
    if (this->mouseEvent)
    {
        return mouseEvent->pos();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->pos();
    }
    else
    {
        // if we land here... the incoming input was
        // neither tablet nor mouse
        return mWidgetPos.toPoint();
    }
}

QPointF PointerEvent::posF() const
{
    if (this->mouseEvent)
    {
        return mouseEvent->localPos();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->posF();
    }
    else
    {
        return mWidgetPos;
    }
}

Qt::MouseButton PointerEvent::button() const
{
    if (this->mouseEvent)
    {
        return this->mouseEvent->button();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->button();
    }
    else
    {
        // if we land here... the incoming input was
        // neither tablet nor mouse
        Q_ASSERT(false);
        return Qt::NoButton;
    }
}

Qt::MouseButtons PointerEvent::buttons() const
{
    if (this->mouseEvent)
    {
        return this->mouseEvent->buttons();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->buttons();
    }
    else
    {
        // if we land here... the incoming input was
        // neither tablet nor mouse
        Q_ASSERT(false);
        return Qt::NoButton;
    }
}

qreal PointerEvent::pressure() const
{
    if (this->tabletEvent)
    {
        return this->tabletEvent->pressure();
    }
    return 1.0;
}

qreal PointerEvent::rotation() const
{
    if (this->tabletEvent)
    {
        return this->tabletEvent->rotation();
    }
    return 0.0;
}

qreal PointerEvent::tangentialPressure() const
{
    if (this->tabletEvent)
    {
        return this->tabletEvent->tangentialPressure();
    }
    return 0.0;
}

int PointerEvent::x() const
{
    if (this->mouseEvent)
    {
        return this->mouseEvent->x();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->x();
    }
    else
    {
        Q_ASSERT(false);
        return 0;
    }

}

int PointerEvent::y() const
{
    if (this->mouseEvent)
    {
        return this->mouseEvent->y();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->y();
    }
    else
    {
        Q_ASSERT(false);
        return 0;
    }
}

bool PointerEvent::isTabletEvent() const
{
    if (this->tabletEvent)
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
    if (this->mouseEvent)
    {
        return this->mouseEvent->modifiers();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->modifiers();
    }
    else
    {
        Q_ASSERT(false);
        return Qt::NoModifier;
    }
}

void PointerEvent::accept()
{
    if (this->mouseEvent)
    {
        this->mouseEvent->accept();
    }
    else if (this->tabletEvent)
    {
        this->tabletEvent->accept();
    }
    else
    {
        Q_ASSERT(false);
    }
}

void PointerEvent::ignore()
{
    if (this->mouseEvent)
    {
        this->mouseEvent->ignore();
    }
    else if (this->tabletEvent)
    {
        this->tabletEvent->ignore();
    }
    else
    {
        Q_ASSERT(false);
    }
}

QEvent::Type PointerEvent::type() const
{
    if (this->mouseEvent)
    {
        return this->mouseEvent->type();
    }
    else if (this->tabletEvent)
    {
        return this->tabletEvent->type();
    }
    return QEvent::None;
}

QTabletEvent::TabletDevice PointerEvent::device() const
{
    if (this->tabletEvent)
    {
        return this->tabletEvent->device();
    }
    return QTabletEvent::TabletDevice::NoDevice;
}

QTabletEvent::PointerType PointerEvent::pointerType() const
{
    if (this->tabletEvent)
    {
        return this->tabletEvent->pointerType();
    }
    return QTabletEvent::PointerType::UnknownPointer;
}

//QEvent::device
