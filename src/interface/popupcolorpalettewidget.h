#ifndef POPUPCOLORPALETTEWIDGET_H
#define POPUPCOLORPALETTEWIDGET_H

#include <QWidget>

class ColorBox;

class PopupColorPaletteWidget : public QWidget
{
  Q_OBJECT

public:
    PopupColorPaletteWidget( QWidget *parent = 0 );
    QColor color;
    bool popup();

protected:
    QWidget* m_container; // the editor, here
    ColorBox* m_colorBox;
    void keyPressEvent(QKeyEvent *event);
};

#endif // POPUPCOLORPALETTEWIDGET_H
