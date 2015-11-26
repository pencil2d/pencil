#ifndef COLORBOX_H
#define COLORBOX_H

#include <QDockWidget>

class ColorWheel;
class ColorInspector;


class ColorBox : public QDockWidget
{
    Q_OBJECT

public:
    explicit ColorBox( const QString& strTitle, QWidget* parent );
    ~ColorBox();

    QColor color();
    void setColor(const QColor &);

Q_SIGNALS:
    void colorChanged(const QColor &);

private:
    void onSpinboxChange(const QColor &);
    void onWheelMove(const QColor &);
    void onWheelRelease(const QColor &);

    ColorWheel* m_colorWheel = nullptr;
    ColorInspector* m_colorInspector = nullptr;
};

#endif // COLORBOX_H
