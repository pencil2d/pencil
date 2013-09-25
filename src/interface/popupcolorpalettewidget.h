#ifndef POPUPCOLORPALETTEWIDGET_H
#define POPUPCOLORPALETTEWIDGET_H

#include <QWidget>
#include "editor.h"

class ColorBox;

class PopupColorPaletteWidget : public QWidget
{
  Q_OBJECT

public:
    PopupColorPaletteWidget( ScribbleArea *parent = 0 );
    //QColor color;
    QPushButton *closeButton;
    ColorBox * getColorBox() { return m_colorBox; }
    void popup();

private slots:
    void onColorChanged(const QColor& color);

protected:
    ScribbleArea *m_container;
    ColorBox *m_colorBox;
    void keyPressEvent(QKeyEvent *event);
};

#endif // POPUPCOLORPALETTEWIDGET_H
