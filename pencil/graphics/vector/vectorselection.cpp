#include "vectorselection.h"

VectorSelection::VectorSelection()
{
}

void VectorSelection::clear()
{
    vertex.clear();
    curve.clear();
}

void VectorSelection::add(int curveNumber)
{
    curve << curveNumber;
}

void VectorSelection::add(QList<int> list)
{
    if (list.size() > 0) add(list[0]);
}

void VectorSelection::add(VertexRef point)
{
    vertex << point;
    add(point.curveNumber);
}

void VectorSelection::add(QList<VertexRef> list)
{
    if (list.size() > 0) add(list[0]);
}
