#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QColor>
#include "basemanager.h"



class ColorManager : public BaseManager
{
    Q_OBJECT
public:
    ColorManager( QObject* parent = 0 );
    ~ColorManager();
    bool initialize() override;

    QColor frontColor();
    int frontColorNumber();

public slots:
    void pickColorNumber(int n);
    void pickColor(const QColor& color);

signals:
    void colorChanged(QColor);
    void colorNumberChanged(int);

private:
    int m_frontColorIndex;

};

#endif // COLORMANAGER_H
