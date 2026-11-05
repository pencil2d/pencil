#include "pointerevent.h"

PointerEvent::PointerEvent(QMouseEvent* event, const QPointF& canvasPos)
{
    mMouseEvent = event;
    mCanvasPos = canvasPos;
}

PointerEvent::PointerEvent(QTabletEvent* event, const QPointF& canvasPos)
{
    mTabletEvent = event;
    mCanvasPos = canvasPos;
}

PointerEvent::~PointerEvent()
{
}

PointerEvent::PointerEvent(const PointerEvent& event, Type type)
    : mType(type), mCanvasPos(event.mCanvasPos)
{
    if (event.mMouseEvent)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        mOwnedMouseEvent.reset(event.mMouseEvent->clone());
#else
        const QMouseEvent* source = event.mMouseEvent;
        mOwnedMouseEvent.reset(new QMouseEvent(source->type(), source->localPos(),
            source->windowPos(), source->screenPos(), source->button(),
            source->buttons(), source->modifiers(), source->source()));
        mOwnedMouseEvent->setTimestamp(source->timestamp());
#endif
        mMouseEvent = mOwnedMouseEvent.get();
    }
    else if (event.mTabletEvent)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        mOwnedTabletEvent.reset(event.mTabletEvent->clone());
#else
        // Qt 5 event copies can share private data deleted by each destructor.
        // Construct an independent event instead of using the implicit copy.
        const QTabletEvent* source = event.mTabletEvent;
        mOwnedTabletEvent.reset(new QTabletEvent(source->type(), source->posF(),
            source->globalPosF(), source->device(), source->pointerType(),
            source->pressure(), source->xTilt(), source->yTilt(),
            source->tangentialPressure(), source->rotation(), source->z(),
            source->modifiers(), source->uniqueId(), source->button(), source->buttons()));
        mOwnedTabletEvent->setTimestamp(source->timestamp());
#endif
        mTabletEvent = mOwnedTabletEvent.get();
    }
}

QPointF PointerEvent::canvasPos() const
{
    return mCanvasPos;
}

QPointF PointerEvent::viewportPos() const
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
    if (mType == Press || mType == Release) { return Qt::LeftButton; }
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
    if (mType == Press) { return Qt::LeftButton; }
    if (mType == Release) { return Qt::NoButton; }
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

bool PointerEvent::isAccepted()
{
    if (mMouseEvent)
    {
        return mMouseEvent->isAccepted();
    }
    else if (mTabletEvent)
    {
        return mTabletEvent->isAccepted();
    }
    Q_ASSERT(false);
    return false;
}

PointerEvent::Type PointerEvent::eventType() const
{
    if (mType != Unmapped) { return mType; }
    if (mMouseEvent)
    {
        switch (mMouseEvent->type())
        {
        case QEvent::MouseButtonPress:
            return Type::Press;
        case QEvent::MouseMove:
            return Type::Move;
        case QEvent::MouseButtonRelease:
            return Type::Release;
        default:
            return Type::Unmapped;
        }
    }
    else if (mTabletEvent)
    {
        switch (mTabletEvent->type())
        {
        case QEvent::TabletPress:
            return Type::Press;
        case QEvent::TabletMove:
            return Type::Move;
        case QEvent::TabletRelease:
            return Type::Release;
        default:
            return Type::Unmapped;
        }
    }
    return Type::Unmapped;
}

PointerEvent::InputType PointerEvent::inputType() const
{
    if (mMouseEvent) {
        return InputType::Mouse;
    }
    else if (mTabletEvent)
    {
        return InputType::Tablet;
    }
    return InputType::Unknown;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

QInputDevice::DeviceType PointerEvent::device() const
{
    if (mTabletEvent)
    {
        return mTabletEvent->deviceType();
    }
    return QInputDevice::DeviceType::Unknown;
}

QPointingDevice::PointerType PointerEvent::pointerType() const
{
    if (mTabletEvent)
    {
        return mTabletEvent->pointerType();
    }
    return QPointingDevice::PointerType::Unknown;
}

#else // QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

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

#endif // QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
