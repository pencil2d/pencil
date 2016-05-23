#ifndef COLORBOX_H
#define COLORBOX_H

#include "basedockwidget.h"

class ColorWheel;
class ColorInspector;


class ColorBox : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit ColorBox( const QString& strTitle, QWidget* parent );
    virtual ~ColorBox();

    void initUI() override;
    void updateUI() override;

    QColor color();
    void setColor(const QColor&);

Q_SIGNALS:
    void colorChanged(const QColor&);

private:
    void onSpinboxChange(const QColor&);
    void onWheelMove(const QColor&);
    void onWheelRelease(const QColor&);

    ColorWheel* mColorWheel = nullptr;
    ColorInspector* mColorInspector = nullptr;
};

#endif // COLORBOX_H
