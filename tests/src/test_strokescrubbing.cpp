// SPDX-License-Identifier: GPL-2.0-only
#include "catch.hpp"

#include <QAction>
#include <QDir>
#include <QSettings>
#include <QTemporaryDir>
#include "bitmapimage.h"
#include "editor.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "object.h"
#include "pointerevent.h"
#include "preferencemanager.h"
#include "scribblearea.h"
#include "stroketool.h"
#include "toolmanager.h"
#include "undoredomanager.h"
#include "vectorimage.h"
#include "viewmanager.h"

namespace {
// ToolManager saves settings on destruction. Keep test preferences isolated.
struct TestSettings
{
    QTemporaryDir directory;
    QSettings::Format previous = QSettings::defaultFormat();
    TestSettings()
    {
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());
    }
    ~TestSettings() { QSettings::setDefaultFormat(previous); }
};

class StrokeCanvas : public ScribbleArea
{
public:
    StrokeCanvas() : ScribbleArea(nullptr) {}
    using ScribbleArea::pointerPressEvent;
    using ScribbleArea::pointerMoveEvent;
    using ScribbleArea::pointerReleaseEvent;
};

struct StrokeFixture
{
    TestSettings settings;
    Editor editor;
    StrokeCanvas canvas;

    explicit StrokeFixture(bool vector = false, bool newUndo = true)
    {
        editor.setScribbleArea(&canvas);
        editor.init();
        editor.preference()->set(SETTING::NEW_UNDO_REDO_SYSTEM_ON, newUndo);
        editor.undoRedo()->init();
        Object* object = new Object;
        object->init();
        Layer* layer = vector ? static_cast<Layer*>(object->addNewVectorLayer())
                              : static_cast<Layer*>(object->addNewBitmapLayer());
        layer->addNewKeyFrameAt(2);
        layer->addNewKeyFrameAt(3);
        editor.setObject(object);
        canvas.setEditor(&editor);
        canvas.init();
        editor.tools()->setCurrentTool(PEN);
        auto tool = static_cast<StrokeTool*>(editor.tools()->currentTool());
        tool->setStablizationLevel(StabilizationLevel::NONE);
        tool->setPressureEnabled(false);
    }

    void mouse(QEvent::Type type, QPointF point)
    {
        Qt::MouseButton button = type == QEvent::MouseMove ? Qt::NoButton : Qt::LeftButton;
        Qt::MouseButtons buttons = type == QEvent::MouseButtonRelease ? Qt::NoButton : Qt::LeftButton;
        QMouseEvent native(type, point, point, button, buttons, Qt::NoModifier);
        PointerEvent event(&native, editor.view()->mapScreenToCanvas(point));
        dispatch(event);
    }

    void dispatch(PointerEvent& event)
    {
        switch (event.eventType())
        {
        case PointerEvent::Press: canvas.pointerPressEvent(&event); break;
        case PointerEvent::Move: canvas.pointerMoveEvent(&event); break;
        case PointerEvent::Release: canvas.pointerReleaseEvent(&event); break;
        default: FAIL("Unexpected pointer event");
        }
    }

    bool empty(int frame)
    {
        auto layer = static_cast<LayerBitmap*>(editor.object()->getLayer(0));
        return layer->getBitmapImageAtFrame(frame)->bounds().isEmpty();
    }
};
}

TEST_CASE("Scrubbing splits a mouse stroke into independently undoable frames", "[stroke][1332]")
{
    bool newUndo = GENERATE(false, true);
    StrokeFixture fixture(false, newUndo);
    fixture.editor.scrubTo(2);
    fixture.mouse(QEvent::MouseButtonPress, { 10, 10 });
    fixture.mouse(QEvent::MouseMove, { 30, 10 });
    fixture.editor.scrubTo(1);
    REQUIRE_FALSE(fixture.empty(2));
    REQUIRE(fixture.empty(1));

    fixture.mouse(QEvent::MouseMove, { 60, 10 });
    fixture.editor.scrubTo(3);
    REQUIRE_FALSE(fixture.empty(1));
    fixture.mouse(QEvent::MouseMove, { 90, 10 });
    fixture.mouse(QEvent::MouseButtonRelease, { 90, 10 });
    REQUIRE_FALSE(fixture.empty(3));

    std::unique_ptr<QAction> undo(fixture.editor.undoRedo()->createUndoAction(nullptr, QIcon()));
    std::unique_ptr<QAction> redo(fixture.editor.undoRedo()->createRedoAction(nullptr, QIcon()));
    fixture.editor.undoRedo()->updateUndoAction(undo.get());
    undo->trigger();
    REQUIRE(fixture.empty(3));
    REQUIRE_FALSE(fixture.empty(1));
    REQUIRE_FALSE(fixture.empty(2));
    if (!newUndo)
    {
        // The legacy stack also keeps checkpoints when changing frames.
        for (int i = 0; i < 8 && (!fixture.empty(1) || !fixture.empty(2)); ++i)
        {
            fixture.editor.undoRedo()->updateUndoAction(undo.get());
            REQUIRE(undo->isEnabled());
            undo->trigger();
        }
        REQUIRE(fixture.empty(1));
        REQUIRE(fixture.empty(2));
        return;
    }
    fixture.editor.undoRedo()->updateUndoAction(undo.get());
    undo->trigger();
    REQUIRE(fixture.empty(1));
    REQUIRE_FALSE(fixture.empty(2));
    fixture.editor.undoRedo()->updateUndoAction(undo.get());
    undo->trigger();
    REQUIRE(fixture.empty(2));
    fixture.editor.undoRedo()->updateRedoAction(redo.get());
    redo->trigger();
    REQUIRE_FALSE(fixture.empty(2));
    REQUIRE(fixture.empty(1));
}

TEST_CASE("Scrubbing to the same frame does not split a stroke", "[stroke][1332]")
{
    StrokeFixture fixture;
    fixture.mouse(QEvent::MouseButtonPress, { 10, 10 });
    fixture.mouse(QEvent::MouseMove, { 30, 10 });
    fixture.editor.scrubTo(0); // clamps to the current frame
    REQUIRE(fixture.empty(1)); // the stroke is still buffered
    fixture.mouse(QEvent::MouseButtonRelease, { 30, 10 });
    REQUIRE_FALSE(fixture.empty(1));
}

TEST_CASE("Vector strokes remain on their original frame when scrubbing", "[stroke][1332]")
{
    StrokeFixture fixture(true);
    auto layer = static_cast<LayerVector*>(fixture.editor.object()->getLayer(0));
    fixture.editor.scrubTo(2);
    fixture.mouse(QEvent::MouseButtonPress, { 10, 10 });
    fixture.mouse(QEvent::MouseMove, { 30, 10 });
    fixture.editor.scrubTo(1);
    REQUIRE_FALSE(layer->getVectorImageAtFrame(2)->isEmpty());
    REQUIRE(layer->getVectorImageAtFrame(1)->isEmpty());
    fixture.mouse(QEvent::MouseMove, { 60, 10 });
    fixture.mouse(QEvent::MouseButtonRelease, { 60, 10 });
    REQUIRE_FALSE(layer->getVectorImageAtFrame(1)->isEmpty());
}

TEST_CASE("Pointer snapshots preserve tablet pressure and device identity", "[stroke][1332]")
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointingDevice device("Test tablet", 12, QInputDevice::DeviceType::Stylus,
        QPointingDevice::PointerType::Pen, QInputDevice::Capability::Position | QInputDevice::Capability::Pressure,
        1, 3);
    QTabletEvent native(QEvent::TabletMove, &device, { 20, 30 }, { 120, 130 },
        0.75, 10, 20, 0.1, 45, 0, Qt::NoModifier, Qt::NoButton, Qt::LeftButton);
#else
    QTabletEvent native(QEvent::TabletMove, { 20, 30 }, { 120, 130 },
        QTabletEvent::Stylus, QTabletEvent::Pen, 0.75, 10, 20, 0.1, 45, 0,
        Qt::NoModifier, 12, Qt::NoButton, Qt::LeftButton);
#endif
    PointerEvent original(&native, { 2, 3 });
    PointerEvent release(original, PointerEvent::Release);
    PointerEvent press(original, PointerEvent::Press);
    REQUIRE(release.inputType() == PointerEvent::Tablet);
    REQUIRE(release.pressure() == Approx(0.75));
    REQUIRE(release.viewportPos() == QPointF(20, 30));
    REQUIRE(release.canvasPos() == QPointF(2, 3));
    REQUIRE(release.device() == original.device());
    REQUIRE(release.pointerType() == original.pointerType());
    REQUIRE(release.button() == Qt::LeftButton);
    REQUIRE(release.buttons() == Qt::NoButton);
    REQUIRE(press.buttons() == Qt::LeftButton);

    StrokeFixture fixture;
    fixture.editor.scrubTo(2);
    fixture.dispatch(press);
    fixture.dispatch(original);
    fixture.editor.scrubTo(1);
    REQUIRE_FALSE(fixture.empty(2));
    fixture.dispatch(release);
    REQUIRE_FALSE(fixture.empty(1));
}
