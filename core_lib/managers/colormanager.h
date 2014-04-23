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
    bool init() override;

    QColor frontColor();
    int frontColorNumber();

public slots:
    void setColor(const QColor& color);
    void setColorNumber( int n );

signals:
    void colorChanged(QColor);
    void colorNumberChanged(int);

private:
    QColor m_currentFrontColor = QColor( 0, 0, 0 );
    int m_currentColorIndex = 0;
};

#endif // COLORMANAGER_H
