#ifndef POPUPCOLORPALETTEWIDGET_H
#define POPUPCOLORPALETTEWIDGET_H

#include <QWidget>

class ColorBox;

class PopupColorPaletteWidget : public QWidget
{
  Q_OBJECT

public:
    PopupColorPaletteWidget( QWidget *parent = 0 );
    ColorBox* m_colorBox;

};

#endif // POPUPCOLORPALETTEWIDGET_H
