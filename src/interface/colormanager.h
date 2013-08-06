#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QObject>
#include <QColor>

class Editor;


class ColorManager : public QObject
{
    Q_OBJECT
public:
    ColorManager(Editor* editor, QObject* parent = 0);
    QColor frontColor();
    int frontColorNumber();

public slots:
    void pickColorNumber(int n);
	void pickColor(const QColor& color);
    
signals:
    void colorChanged(QColor);
    void colorNumberChanged(int);

private:
    Editor* m_editor;
    int m_frontColorIndex;
};

#endif // COLORMANAGER_H
