#ifndef COLORSPINBOXGROUP_H
#define COLORSPINBOXGROUP_H

#include <QWidget>

namespace Ui {
class ColorInspector;
}

class ColorInspector : public QWidget
{
    Q_OBJECT
    
public:
    explicit ColorInspector(QWidget *parent = 0);
    ~ColorInspector();
    QColor color();
signals:
    void colorChanged(const QColor& c);
    void modeChange(const bool& isRgb);

public slots:
    void setColor(const QColor &c);

private slots:
    void onModeChanged();
    void onColorChanged();
    
private:
    Ui::ColorInspector *ui;
    bool isRgbColors;
    bool noColorUpdate;
    QColor m_color;
};

#endif // COLORSPINBOXGROUP_H
