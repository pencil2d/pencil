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
    QPushButton* closeButton;
    ColorBox* getColorBox() { return m_pColorBox; }
    void popup();

private slots:
    void onColorChanged(const QColor& color);

protected:
    ScribbleArea* m_pContainer;
    ColorBox* m_pColorBox;
    void keyPressEvent(QKeyEvent *event);
};

#endif // POPUPCOLORPALETTEWIDGET_H
