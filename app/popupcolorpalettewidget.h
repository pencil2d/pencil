#ifndef POPUPCOLORPALETTEWIDGET_H
#define POPUPCOLORPALETTEWIDGET_H

#include <QWidget>

class ColorBox;
class ScribbleArea;
class QPushButton;


class PopupColorPaletteWidget : public QWidget
{
    Q_OBJECT

public:
    PopupColorPaletteWidget( ScribbleArea *parent = 0 );
    QPushButton* closeButton;
    ColorBox* getColorBox() { return mColorBox; }
    void popup();

private slots:
    void onColorChanged(const QColor& color);

protected:
    ScribbleArea* mContainer;
    ColorBox* mColorBox = nullptr;
    void keyPressEvent(QKeyEvent *event);
};

#endif // POPUPCOLORPALETTEWIDGET_H
