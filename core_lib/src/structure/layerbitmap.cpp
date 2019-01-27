/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "layerbitmap.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QColor>
#include "keyframe.h"
#include "bitmapimage.h"
#include <QtMath>


LayerBitmap::LayerBitmap(Object* object) : Layer(object, Layer::BITMAP)
{
    setName(tr("Bitmap Layer"));
}

LayerBitmap::~LayerBitmap()
{
}

BitmapImage* LayerBitmap::getBitmapImageAtFrame(int frameNumber)
{
    Q_ASSERT(frameNumber >= 1);
    return static_cast<BitmapImage*>(getKeyFrameAt(frameNumber));
}

BitmapImage* LayerBitmap::getLastBitmapImageAtFrame(int frameNumber, int increment)
{
    Q_ASSERT(frameNumber >= 1);
    return static_cast<BitmapImage*>(getLastKeyFrameAtPosition(frameNumber + increment));
}

void LayerBitmap::initColorLayer(Layer *fromLayer, LayerBitmap *colorlayer)
{
    int max = fromLayer->getMaxKeyFramePosition();
    for (int i = 1; i <=max; i++)
    {
        colorlayer->copyFrame(fromLayer, colorlayer, i);
        toBlackLine(i);
    }
}

BitmapImage* LayerBitmap::scanToTransparent(int frame)
{
    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    int xOffset = img->left();
    int yOffset = img->top();
    for (int x = 0; x < img->width(); x++)
    {
        for (int y = 0; y < img->height(); y++)
        {
            QColor c = img->pixel(x + xOffset, y + yOffset);
            if (c.value() >= mThreshold)
            {
                img->setPixel(x + xOffset, y + yOffset, transp);
            }
            else if(c.value() > 99 && c.value() < mThreshold)
            {
                int alpha = static_cast<int>(floor(255 - 255/(mThreshold - 100) * (c.value() - 100)));
                QRgb rgba = qRgba(c.value(), c.value(), c.value(), alpha);
                img->setPixel(x + xOffset, y + yOffset, rgba);
            }
        }
    }
    qDebug() << img->bounds();
    return img;
}

void LayerBitmap::toBlackLine(int frame)
{
    if (!keyExists(frame)) { return; }

    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    int xOffset = img->left();
    int yOffset = img->top();

    // make line black (0, 1, 0, 255)
    for (int x = 0; x < img->width(); x++)
    {
        for (int y = 0; y < img->height(); y++)
        {
            QRgb rgba = img->pixel(x + xOffset, y + yOffset);
            if (rgba != transp)
                img->setPixel(x + xOffset, y + yOffset, thinline);
        }
    }
}

void LayerBitmap::fillWhiteAreas(int frame)
{
    if (!keyExists(frame)) { return; }

    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    int xOffset = img->left();
    int yOffset = img->top();

    // fill areas size 'area' or less with black
    QVector<QPoint> points;
    points.clear();
    for (int x = 0; x < img->width(); x++)
    {
        for (int y = 0; y < img->height(); y++)
        {
            if (qAlpha(img->pixel(x + xOffset, y + yOffset)) == 0)
            {
                points.append(QPoint(x + xOffset, y + yOffset));
                int areaSize = fillWithColor(QPoint(x + xOffset, y + yOffset), transp, rosa, frame);
                if (areaSize <= mWhiteArea)
                {   // replace rosa with thinline (black)
                    fillWithColor(points.last(), rosa, thinline, frame);
                    points.removeLast();
                }
            }
        }
    }
    // replace rosa with trans
    while (!points.isEmpty()) {
        fillWithColor(points[0], rosa, transp, frame);
        points.removeFirst();
    }
}

void LayerBitmap::toThinBlackLine(int frame)
{
    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    int teller = 0, infound = 0, inElse = 0;
    bool N = true, E = true, S = true, W = true, black, search;
    while (N || E || S || W)
    {
        if (N)  // from NORTH
        {
            // set 'black' to false. 'black' is set to true whenever a black pixel is removed
            black = false;
            // 'search' is true while pixels are transparent
            // when thinline pixel is found, 'search' is set to false until next transparent pixel
            search = true;
            for (int x = img->left(); x <= img->right(); x++)
            {
                for (int y = img->top(); y < img->bottom(); y++)
                {
                    if (search && qAlpha(img->pixel(x, y)) > 0)
                    {
                        infound++;
                        qDebug() << "x,y+1: " << qAlpha(img->pixel(x, y+1));
                        if (qAlpha(img->pixel(x, y+1) > 0) &&
                                (qAlpha(img->pixel(x+1, y)) > 0 || qAlpha(img->pixel(x+1, y+1)) > 0 ||
                                qAlpha(img->pixel(x-1, y)) > 0 || qAlpha(img->pixel(x-1, y+1)) > 0))
                        {
                            img->setPixel(x, y, transp);
                            black = true;
                            search = false;
                            teller++;
                        }
                    }
                    else
                    {
                        inElse++;
                        if (qAlpha(img->pixel(x,y)) == 0)
                            search = true;
                    }
                }
            }
            N = black; // if none 'black' is removed, N = false
        }
        if (E)  // from EAST
        {
            black = false;
            search = true;
            for (int y = img->top(); y <= img->bottom(); y++)
            {
                for (int x = img->right(); x > img->left(); x--)
                {
                    if (search && qAlpha(img->pixel(x, y)) > 0)
                    {
                        infound++;
                        if (qAlpha(img->pixel(x-1, y) > 0) &&
                                (qAlpha(img->pixel(x-1, y-1)) > 0 || qAlpha(img->pixel(x, y-1)) > 0 ||
                                qAlpha(img->pixel(x-1, y+1)) > 0 || qAlpha(img->pixel(x, y+1)) > 0))
                        {
                            img->setPixel(x, y, transp);
                            black = true;
                            search = false;
                            teller++;
                        }
                    }
                    else
                    {
                        inElse++;
                        if (qAlpha(img->pixel(x,y)) == 0)
                            search = true;
                    }
                }
            }
            E = black; // if none 'black' is removed, E = false
        }
        if (S)  // from SOUTH
        {
            black = false;
            search = true;
            for (int x = img->left(); x <= img->right(); x++)
            {
                for (int y = img->bottom(); y > img->top(); y--)
                {
                    if (search && qAlpha(img->pixel(x, y)) > 0)
                    {
                        infound++;
                        if (qAlpha(img->pixel(x, y-1) > 0) &&
                                ((qAlpha(img->pixel(x-1, y-1)) > 0 || qAlpha(img->pixel(x-1, y)) > 0 ||
                                qAlpha(img->pixel(x+1, y-1)) > 0 || qAlpha(img->pixel(x+1, y)) > 0)))
                        {
                            img->setPixel(x, y, transp);
                            black = true;
                            search = false;
                            teller++;
                        }
                    }
                    else
                    {
                        inElse++;
                        if (qAlpha(img->pixel(x,y)) == 0)
                            search = true;
                    }
                }
            }
            S = black; // if none 'black' is removed, S = false
        }
        if (W)  // from WEST
        {
            black = false;
            search = true;
            for (int y = img->top(); y <= img->bottom(); y++)
            {
                for (int x = img->left(); x < img->right(); x++)
                {
                    if (search && qAlpha(img->pixel(x, y)) > 0)
                    {
                        infound++;
                        if (qAlpha(img->pixel(x+1, y) > 0) &&
                                (qAlpha(img->pixel(x, y-1)) > 0 || qAlpha(img->pixel(x+1, y-1)) > 0 ||
                                 qAlpha(img->pixel(x, y+1)) > 0 || qAlpha(img->pixel(x+1, y+1)) > 0))
                        {
                            img->setPixel(x, y, transp);
                            black = true;
                            search = false;
                            teller++;
                        }
                        else
                        {
                            inElse++;
                            if (qAlpha(img->pixel(x,y)) == 0)
                                search = true;
                        }
                    }
                    W = black; // if none 'black' is removed, E = false
                }
            }
        }
    }
    qDebug() << "Finished thin line. Replaced :" << teller;
    qDebug() << "Finished thin line. In found :" << infound;
    qDebug() << "Finished thin line. In else  :" << inElse;
}

int LayerBitmap::fillWithColor(QPoint point, QRgb orgColor, QRgb newColor, int frame)
{
    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    QList<QPoint> fillList;
    fillList.clear();
    // fill first pixel
    img->setPixel(point, newColor);
    int pixels = 1;
    fillList.append(point);

    QRect rect = img->bounds();
    while (!fillList.isEmpty())
    {
        QPoint tmp = fillList.at(0);
        if (rect.contains(QPoint(tmp.x() + 1, tmp.y())) && img->pixel(QPoint(tmp.x() + 1, tmp.y())) == orgColor)
        {
            img->setPixel(QPoint(tmp.x() + 1, tmp.y()), newColor);
            fillList.append(QPoint(tmp.x() + 1, tmp.y()));
            pixels++;
        }
        if (rect.contains(QPoint(tmp.x(), tmp.y() + 1)) && img->pixel(QPoint(tmp.x(), tmp.y() + 1)) == orgColor)
        {
            img->setPixel(QPoint(tmp.x(), tmp.y() + 1), newColor);
            fillList.append(QPoint(tmp.x(), tmp.y() + 1));
            pixels++;
        }
        if (rect.contains(QPoint(tmp.x() - 1, tmp.y())) && img->pixel(QPoint(tmp.x() - 1, tmp.y())) == orgColor)
        {
            img->setPixel(QPoint(tmp.x() - 1, tmp.y()), newColor);
            fillList.append(QPoint(tmp.x() - 1, tmp.y()));
            pixels++;
        }
        if (rect.contains(QPoint(tmp.x(), tmp.y() - 1)) && img->pixel(QPoint(tmp.x(), tmp.y() - 1)) == orgColor)
        {
            img->setPixel(QPoint(tmp.x(), tmp.y() - 1), newColor);
            fillList.append(QPoint(tmp.x(), tmp.y() - 1));
            pixels++;
        }
        fillList.removeFirst();
    }
    return pixels;
}

void LayerBitmap::loadImageAtFrame(QString path, QPoint topLeft, int frameNumber)
{
    BitmapImage* pKeyFrame = new BitmapImage(topLeft, path);
    pKeyFrame->setPos(frameNumber);
    loadKey(pKeyFrame);
}

Status LayerBitmap::saveKeyFrameFile(KeyFrame* keyframe, QString path)
{
    QString strFilePath = filePath(keyframe, QDir(path));

    BitmapImage* bitmapImage = static_cast<BitmapImage*>(keyframe);

    bool needSave = needSaveFrame(keyframe, strFilePath);
    if (!needSave)
    {
        return Status::SAFE;
    }

    bitmapImage->setFileName(strFilePath);

    Status st = bitmapImage->writeFile(strFilePath);
    if (!st.ok())
    {
        bitmapImage->setFileName("");

        DebugDetails dd;
        dd << "LayerBitmap::saveKeyFrame";
        dd << QString("  KeyFrame.pos() = %1").arg(keyframe->pos());
        dd << QString("  strFilePath = %1").arg(strFilePath);
        dd << QString("BitmapImage could not be saved");
        dd.collect(st.details());
        return Status(Status::FAIL, dd);
    }

    bitmapImage->setModified(false);
    return Status::OK;
}

KeyFrame* LayerBitmap::createKeyFrame(int position, Object*)
{
    BitmapImage* b = new BitmapImage;
    b->setPos(position);
    b->enableAutoCrop(true);
    return b;
}

Status LayerBitmap::presave(const QString& sDataFolder)
{
    QDir dataFolder(sDataFolder);
    // Handles keys that have been moved but not modified
    std::vector<BitmapImage*> movedOnlyBitmaps;
    foreachKeyFrame([&movedOnlyBitmaps,&dataFolder,this](KeyFrame* key)
    {
        auto bitmap = static_cast<BitmapImage*>(key);
        // (b->fileName() != fileName(b) && !modified => the keyframe has been moved, but users didn't draw on it.
        if (!bitmap->fileName().isEmpty()
            && !bitmap->isModified()
            && bitmap->fileName() != filePath(bitmap, dataFolder))
        {
            movedOnlyBitmaps.push_back(bitmap);
        }
    });

    for (BitmapImage* b : movedOnlyBitmaps)
    {
        // Move to temporary locations first to avoid overwritting anything we shouldn't be
        // Ex: Frame A moves from 1 -> 2, Frame B moves from 2 -> 3. Make sure A does not overwrite B
        QString tmpName = QString::asprintf("t_%03d.%03d.png", id(), b->pos());
        QDir sA, sB;
        if ((sA=QFileInfo(b->fileName()).dir()) != (sB=dataFolder)) {
            // Copy instead of move if the data folder itself has changed
            QFile::copy(b->fileName(), tmpName);
        }
        else {
            QFile::rename(b->fileName(), tmpName);
        }
        b->setFileName(tmpName);
    }

    for (BitmapImage* b : movedOnlyBitmaps)
    {
        QString dest = filePath(b, dataFolder);
        QFile::remove(dest);

        QFile::rename(b->fileName(), dest);
        b->setFileName(dest);
    }

    return Status::OK;
}

QString LayerBitmap::filePath(KeyFrame* key, const QDir& dataFolder) const
{
    return dataFolder.filePath(fileName(key));
}

QString LayerBitmap::fileName(KeyFrame* key) const
{
    return QString::asprintf("%03d.%03d.png", id(), key->pos());
}

bool LayerBitmap::needSaveFrame(KeyFrame* key, const QString& strSavePath)
{
    if (key->isModified()) // keyframe was modified
        return true;
    if (QFile::exists(strSavePath) == false) // hasn't been saved before
        return true;
    return false;
}

QDomElement LayerBitmap::createDomElement(QDomDocument& doc)
{
    QDomElement layerTag = doc.createElement("layer");
    layerTag.setAttribute("id", id());
    layerTag.setAttribute("name", name());
    layerTag.setAttribute("visibility", visible());
    layerTag.setAttribute("type", type());

    foreachKeyFrame([&](KeyFrame* pKeyFrame)
    {
        BitmapImage* pImg = static_cast<BitmapImage*>(pKeyFrame);

        QDomElement imageTag = doc.createElement("image");
        imageTag.setAttribute("frame", pKeyFrame->pos());
        imageTag.setAttribute("src", fileName(pKeyFrame));
        imageTag.setAttribute("topLeftX", pImg->topLeft().x());
        imageTag.setAttribute("topLeftY", pImg->topLeft().y());
        layerTag.appendChild(imageTag);

        Q_ASSERT(QFileInfo(pKeyFrame->fileName()).fileName() == fileName(pKeyFrame));
    });

    return layerTag;
}

void LayerBitmap::loadDomElement(QDomElement element, QString dataDirPath, ProgressCallback progressStep)
{
    if (!element.attribute("id").isNull())
    {
        int id = element.attribute("id").toInt();
        setId(id);
    }
    setName(element.attribute("name"));
    setVisible(element.attribute("visibility").toInt() == 1);

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "image")
            {
                QString path = dataDirPath + "/" + imageElement.attribute("src"); // the file is supposed to be in the data directory
                QFileInfo fi(path);
                if (!fi.exists()) path = imageElement.attribute("src");
                int position = imageElement.attribute("frame").toInt();
                int x = imageElement.attribute("topLeftX").toInt();
                int y = imageElement.attribute("topLeftY").toInt();
                loadImageAtFrame(path, QPoint(x, y), position);

                progressStep();
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
