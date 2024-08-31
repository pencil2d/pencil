#ifndef TIMELINELAYERHEADERCELL_H
#define TIMELINELAYERHEADERCELL_H

#include "timeline.h"
#include "editor.h"

#include "timelinebasecell.h"

#include <QPainter>

class TimeLineLayerHeaderCell : public TimeLineBaseCell
{
public:
    TimeLineLayerHeaderCell(TimeLine* timeLine, 
                            Editor* editor,
                            QPoint origin, 
                            int width, 
                            int height);
                            
    ~TimeLineLayerHeaderCell() override;
    
    TimeLineCellType type() const override { return TimeLineCellType::HEADER; }

    void paintGlobalDotVisibility(QPainter& painter, const QPalette& palette) const;
    void paintSplitter(QPainter& painter, const QPalette& palette) const;

    void mousePressEvent(QMouseEvent* event) override;
    // void mouseMoveEvent(QMouseEvent* event) override;
    // void mouseReleaseEvent(QMouseEvent* event) override;
    void paint(QPainter &painter, const QPalette &palette) const override;

private:
};

#endif // TIMELINELAYERHEADERCELL_H
