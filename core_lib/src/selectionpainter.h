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

    void paint(QPainter& painter, const Object* object, int layerIndex, BaseTool* tool, TransformParameters& transformParameters, float scaling);

    void setOriginalPolygonF(QPolygonF polyF) { mOriginalSelectionPolygonF = polyF; }
    void setOriginalPolygonFIsSet(bool b) { mOriginalPolygonFIsSet = b; }
    bool originalPolygonFIsSet() { return mOriginalPolygonFIsSet; }

private:
    QPolygonF mOriginalSelectionPolygonF = QPolygonF();
    bool mOriginalPolygonFIsSet = false;
};

#endif // SelectionPainter_H
