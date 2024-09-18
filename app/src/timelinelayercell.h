#ifndef TIMELINELAYERCELL_H
#define TIMELINELAYERCELL_H

#include <QWidget>
#include <QPainter>
#include <QPalette>

#include "timeline.h"
#include "editor.h"
#include "timelinebasecell.h"

class Layer;
class LayerCamera;
class TimeLineLayerCellEditorWidget;

class PreferenceManager;

class TimeLineLayerCell : public TimeLineBaseCell
{
    Q_OBJECT
public:
    TimeLineLayerCell(TimeLine* timeline,
                      QWidget* parent,
                      Editor* editor,
                      Layer* layer,
                      const QPoint& origin, int width, int height);
    ~TimeLineLayerCell() override;

    TimeLineCellType type() const override { return TimeLineCellType::LAYER; }

    TimeLineLayerCellEditorWidget* editorWidget() const { return mEditorWidget; }

private:
    TimeLineLayerCellEditorWidget* mEditorWidget = nullptr;
};

#endif // TIMELINELAYERCELL_H
