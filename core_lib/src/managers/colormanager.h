/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QColor>
#include "basemanager.h"



class ColorManager : public BaseManager
{
    Q_OBJECT
public:
    explicit ColorManager(Editor* editor);
    ~ColorManager() override;

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;
    void workingLayerChanged(Layer*) override;

    /** frontColor
     * @param useIndexedColor default true, will only affect vector layer
     * @return Latest stored color for bitmap or indexed color for vector, unless useIndexedColor is false
     */
    QColor frontColor(bool useIndexedColor = true);
    void setFrontColor(const QColor& newFrontColor);

    /** Set color for current index */
    void setIndexedColor(const QColor& newColor);
    void setColorNumber(int n);
    int frontColorNumber() const;

signals:
    void colorChanged(QColor, int); // new color and color index
    void colorNumberChanged(int);

private:
    QColor mCurrentFrontColor{ 33, 33, 33, 255 };
    int  mCurrentColorIndex = 0;
    bool mIsWorkingOnVectorLayer = false;
};

#endif // COLORMANAGER_H
