#ifndef VECTORSELECTION_H
#define VECTORSELECTION_H


#include <QList>
#include "vertexref.h"

class VectorSelection
{
public:
    VectorSelection();
    QList<VertexRef> vertex;
    QList<int> curve;
    //QList<int> area;
    void clear();
    void add(int curveNumber);
    void add(QList<int> curveNumbers);
    void add(VertexRef point);
    void add(QList<VertexRef> points);
};

#endif // VECTORSELECTION_H
