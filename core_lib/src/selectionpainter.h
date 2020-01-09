#ifndef SelectionPainter_H
#define SelectionPainter_H

#include "QRectF"
#include "QPolygonF"

class QPainter;
class Object;
class BaseTool;

struct TransformParameters
{
    QPolygonF lastSelectionPolygonF;
    QPolygonF currentSelectionPolygonF;
};

class SelectionPainter
{
public:
    SelectionPainter();

    void paint(QPainter& painter, const Object* object, int layerIndex, BaseTool* tool, TransformParameters& transformParameters);
    void setShowInfo(int index) { mShowInfo = index; };

private:
    int mShowInfo = 0;
    QPolygonF mOriginalSelectionPolygonF = QPolygonF();
};

#endif // SelectionPainter_H
