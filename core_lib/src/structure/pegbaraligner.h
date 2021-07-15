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
#ifndef PEGBARALIGNER_H
#define PEGBARALIGNER_H

#include <pencilerror.h>

#include <QPoint>
#include <QRectF>

class BitmapImage;
class Editor;

class PegStatus : public Status
{
public:
    PegStatus(ErrorCode code, QPoint point = {});
    QPoint point;
};

class PegBarAligner
{
    Q_DECLARE_TR_FUNCTIONS(PegBarAligner)
public:
    PegBarAligner(Editor* editor, QRect searchRect);

    Status align(const QStringList& layers);

private:
    PegStatus findPoint(const BitmapImage& image) const;

    Editor* mEditor = nullptr;

    const int mGrayThreshold = 121;
    QRect mPegSearchRect;
};

#endif // PEGBARALIGNER_H
