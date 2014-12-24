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
    void setColor(const QColor& color);
    void setColorNumber( int n );

Q_SIGNALS:
    void colorChanged(QColor);
    void colorNumberChanged(int);

private:
    QColor mCurrentFrontColor = { 0, 0, 0 };
    int mCurrentColorIndex = 0;
};

#endif // COLORMANAGER_H
