#ifndef POINTEREVENT_H
#define POINTEREVENT_H

#include <QTabletEvent>
#include <QMouseEvent>

class PointerEvent
{
public:
    enum InputType {
        Mouse,
        Tablet,
        Touch,
        Unknown
    };

    enum Type {
        Press,
        Move,
        Release,
        Unmapped
    };

    PointerEvent(QMouseEvent* event, const QPointF& canvasPos);
    PointerEvent(QTabletEvent* event, const QPointF& canvasPos);
    ~PointerEvent();

    /**
     * Returns the QPointF of the device, in canvas coordinates
     */
    QPointF canvasPos() const;

    /**
     * Returns the QPointF of the device, in viewport coordinates
     * Returns pos() if used on mouse event
     */
    QPointF viewportPos() const;

    /**
     * Returns a value between 0 and 1 for tablet events,
     * otherwise 1.0
     */
    qreal pressure() const;

    /**
     * Returns rotation value if any, otherwise 0 */
    qreal rotation() const;

    /**
     * Returns the tangential pressure of a tablet's that support it
     * This is typically given by a finger wheel on an airbrush tool. The range
     * is from -1.0 to 1.0. 0.0 indicates a neutral position. Current airbrushes can
     * only move in the positive direction from the neutral position. If the device
     * does not support tangential pressure, this value is always 0.0.
     */
    qreal tangentialPressure() const;

    /** Returns the x position of the input device in the widget */
    int x() const;

    /** Returns the y position of the input device in the widget */
    int y() const;

    /** Returns true if the device was tablet, otherwise false */
    bool isTabletEvent() const;

    /** Returns the modifier created by keyboard while a device was in use */
    Qt::KeyboardModifiers modifiers() const;

    /** Returns Qt::MouseButton() */
    Qt::MouseButton button() const;

    /** Returns Qt::MouseButtons() */
    Qt::MouseButtons buttons() const;

    void accept();
    void ignore();

    bool isAccepted();

    Type eventType() const;
    InputType inputType() const;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QInputDevice::DeviceType device() const;
    QPointingDevice::PointerType pointerType() const;
#else
    QTabletEvent::TabletDevice device() const;
    QTabletEvent::PointerType pointerType() const;
#endif

private:
    QTabletEvent* mTabletEvent = nullptr;
    QMouseEvent* mMouseEvent = nullptr;
    QPointF mCanvasPos;
};

#endif // POINTEREVENT_H
