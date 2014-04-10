/****************************************************************************
**
** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>

class QImage;
class QPainter;
class QWidget;
class QPainterPath;
class QPoint;
class QRect;
class QStringList;

class QMatrix;
class QSize;
class QString;

class Object;

class BrushInterface
{
public:
    virtual ~BrushInterface() {}

    virtual QStringList brushes() const = 0;
    virtual QRect mousePress(const QString& brush, QPainter& painter,
                             const QPoint& pos) = 0;
    virtual QRect mouseMove(const QString& brush, QPainter& painter,
                            const QPoint& oldPos, const QPoint& newPos) = 0;
    virtual QRect mouseRelease(const QString& brush, QPainter& painter,
                               const QPoint& pos) = 0;
};

class ShapeInterface
{
public:
    virtual ~ShapeInterface() {}

    virtual QStringList shapes() const = 0;
    virtual QPainterPath generateShape(const QString& shape,
                                       QWidget* parent) = 0;
};

class FilterInterface
{
public:
    virtual ~FilterInterface() {}

    virtual QStringList filters() const = 0;
    virtual QImage filterImage(const QString& filter, const QImage& image,
                               QWidget* parent) = 0;
};

class ExportInterface
{
public:
    virtual ~ExportInterface() {}
    virtual QString name() const = 0;
    virtual bool exportFile(Object* object, int startFrame, int endFrame, QMatrix view, QSize exportSize, QString filePath, int fps, int compression) const = 0;
    /*virtual QImage filterImage(const QString &filter, const QImage &image,
                               QWidget *parent) = 0;*/
};

Q_DECLARE_INTERFACE(BrushInterface,
                    "com.trolltech.PlugAndPaint.BrushInterface/1.0")
Q_DECLARE_INTERFACE(ShapeInterface,
                    "com.trolltech.PlugAndPaint.ShapeInterface/1.0")
Q_DECLARE_INTERFACE(FilterInterface,
                    "com.trolltech.PlugAndPaint.FilterInterface/1.0")
Q_DECLARE_INTERFACE(ExportInterface,
                    "org.pencil.plugins.ExportInterface/1.0")

#endif
