#ifndef BLITRECT_H
#define BLITRECT_H

#include <QRect>
#include <QPoint>

class BlitRect : public QRect
{
public:
    explicit BlitRect();

    void extend(QPoint p);

protected:
    int points;
};

#endif // BLITRECT_H
