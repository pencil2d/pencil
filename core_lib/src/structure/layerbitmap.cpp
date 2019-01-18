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

QRect LayerBitmap::getUpdatedBounds(int frame)
{
    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
//    qDebug() << "L: " << img->left() << ", R: " << img->right() << ", T: " << img->top() << ", B: " << img->bottom();
    if (img == nullptr) return QRect(0, 0, 0, 0);
    int aleft = 0, aright = 0, atop = 0, abottom = 0;

    bool cont = true;
    // find aleft
    for (int x = img->left(); x < img->right(); x++)
    {
        if (!cont)
            break;
        for (int y = img->top(); y < img->bottom(); y++)
        {
            QColor c = img->pixel(x, y);
            if (c.value() < 211)
            {
                aleft = x;
                cont = false;
            }
        }
    }
    cont = true;

    // find aright
    for (int x = img->right(); x > img->left(); x--)
    {
        if (!cont)
            break;
        for (int y = img->top(); y < img->bottom(); y++)
        {
            QColor c = img->pixel(x, y);
            if (c.value() < 211)
            {
                aright = x;
                cont = false;
            }
        }
    }
    cont = true;

    // find atop
    for (int y = img->top(); y < img->bottom(); y++)
    {
        if (!cont)
            break;
        for (int x = img->left(); x < img->right(); x++)
        {
            QColor c = img->pixel(x, y);
            if (c.value() < 211)
            {
                atop = y;
                cont = false;
            }
        }
    }
    cont = true;

    // find abottom
    for (int y = img->bottom(); y > img->top(); y--)
    {
        if (!cont)
            break;
        for (int x = img->left(); x < img->right(); x++)
        {
            QColor c = img->pixel(x, y);
            if (c.value() < 211)
            {
                abottom = y;
                cont = false;
            }
        }
    }
    return QRect(aleft, atop, aright - aleft, abottom - atop);
}

BitmapImage *LayerBitmap::toTransparentScan(int frame)
{
    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    qDebug() << "OLD bounds: " << img->bounds();
    img->setBounds(getUpdatedBounds(frame));
    Q_ASSERT(img != nullptr);
    qDebug() << "New bounds: " << img->bounds();

    int xOffset = img->left();
    int yOffset = img->top();
    QRgb transp = qRgba(0, 0, 0, 0);
    for (int x = 0; x < img->width(); x++)
    {
        for (int y = 0; y < img->height(); y++)
        {
            QColor c = img->pixel(x + xOffset, y + yOffset);
            if (c.value() > mThreshold)
                img->setPixel(x + xOffset, y + yOffset, transp);
        }
    }
    return img;
}

void LayerBitmap::toBlackLine(int frame, int area = 5)
{
    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    int xOffset = img->left();
    int yOffset = img->top();
    QRgb black = qRgba(0, 1, 0, 255);

    // make line black (0, 1, 0, 255)
    for (int x = 0; x < img->width(); x++)
    {
        for (int y = 0; y < img->height(); y++)
        {
            QColor c = img->pixel(x + xOffset, y + yOffset);
            if (c.value() < 211)
                img->setPixel(x + xOffset, y + yOffset, black);
        }
    }

    // fill areas size 5 or less with black
    QVector<QPoint> points;
    points.clear();
    for (int x = 0; x < img->width(); x++)
    {
        for (int y = 0; y < img->height(); y++)
        {
            if (qAlpha(img->pixel(x,y)) == 255)
            {
                points.append(QPoint(x + xOffset, y + yOffset));
                // hold øje med om det er et hul osv...
            }
        }
    }
}

void LayerBitmap::toThinBlackLine(int frame)
{
    BitmapImage* img = static_cast<BitmapImage*>(getKeyFrameAt(frame));
    int xOffset = img->left();
    int yOffset = img->top();
    QRgb thinline = qRgba(0, 1, 0, 255);
    for (int x = 0; x < img->width(); x++)
    {
        for (int y = 0; y < img->height(); y++)
        {
            QColor c = img->pixel(x + xOffset, y + yOffset);
            if (c.value() > 210)
            {

            }
        }
    }
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
