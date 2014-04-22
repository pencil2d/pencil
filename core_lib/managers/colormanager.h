#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QColor>
#include "basemanager.h"



class ColorManager : public BaseManager
{
    Q_OBJECT
public:
    ColorManager( QObject* parent );
    ~ColorManager();
    bool initialize() override;

    QColor frontColor();
    int frontColorNumber();

public slots:
    void setColor(const QColor& color);
    void setColorNumber( int n );

signals:
    void colorChanged(QColor);
    void colorNumberChanged(int);

private:
    QColor m_currentFrontColor;
    int m_currentColorIndex;
};

#endif // COLORMANAGER_H
