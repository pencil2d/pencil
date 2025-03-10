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
#include "layerbitmap.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include "keyframe.h"
#include "bitmapimage.h"
#include "util/util.h"

LayerBitmap::LayerBitmap(int id) : Layer(id, Layer::BITMAP)
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

void LayerBitmap::replaceKeyFrame(const KeyFrame* bitmapImage)
{
    *getBitmapImageAtFrame(bitmapImage->pos()) = *static_cast<const BitmapImage*>(bitmapImage);
}

void LayerBitmap::repositionFrame(QPoint point, int frame)
{
    BitmapImage* image = getBitmapImageAtFrame(frame);
    Q_ASSERT(image);
    image->moveTopLeft(point);
}

QRect LayerBitmap::getFrameBounds(int frame)
{
    BitmapImage* image = getBitmapImageAtFrame(frame);
    Q_ASSERT(image);
    return image->bounds();
}

void LayerBitmap::loadImageAtFrame(QString path, QPoint topLeft, int frameNumber, qreal opacity)
{
    BitmapImage* pKeyFrame = new BitmapImage(topLeft, path);
    pKeyFrame->enableAutoCrop(true);
    pKeyFrame->setPos(frameNumber);
    pKeyFrame->setOpacity(opacity);
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
        dd << QString("Error: Failed to save BitmapImage");
        dd.collect(st.details());
        return Status(Status::FAIL, dd);
    }

    bitmapImage->setModified(false);
    return Status::OK;
}

KeyFrame* LayerBitmap::createKeyFrame(int position)
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
        QString tmpPath = dataFolder.filePath(QString::asprintf("t_%03d.%03d.png", id(), b->pos()));
        if (QFileInfo(b->fileName()).dir() != dataFolder) {
            // Copy instead of move if the data folder itself has changed
            QFile::copy(b->fileName(), tmpPath);
        }
        else {
            QFile::rename(b->fileName(), tmpPath);
        }
        b->setFileName(tmpPath);
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

bool LayerBitmap::needSaveFrame(KeyFrame* key, const QString& savePath)
{
    if (key->isModified()) // keyframe was modified
        return true;
    if (QFile::exists(savePath) == false) // hasn't been saved before
        return true;
    if (key->fileName().isEmpty())
        return true;
    return false;
}

QDomElement LayerBitmap::createDomElement(QDomDocument& doc) const
{
    QDomElement layerElem = createBaseDomElement(doc);

    foreachKeyFrame([&](KeyFrame* pKeyFrame)
    {
        BitmapImage* pImg = static_cast<BitmapImage*>(pKeyFrame);

        QDomElement imageTag = doc.createElement("image");
        imageTag.setAttribute("frame", pKeyFrame->pos());
        imageTag.setAttribute("src", fileName(pKeyFrame));
        imageTag.setAttribute("topLeftX", pImg->topLeft().x());
        imageTag.setAttribute("topLeftY", pImg->topLeft().y());
        imageTag.setAttribute("opacity", pImg->getOpacity());
        layerElem.appendChild(imageTag);

        if (!pKeyFrame->fileName().isEmpty()) {
            Q_ASSERT(QFileInfo(pKeyFrame->fileName()).fileName() == fileName(pKeyFrame));
        }
    });

    return layerElem;
}

void LayerBitmap::loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep)
{
    this->loadBaseDomElement(element);

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull() && imageElement.tagName() == "image")
        {
            QString path = validateDataPath(imageElement.attribute("src"), dataDirPath);
            if (!path.isEmpty())
            {
                int position = imageElement.attribute("frame").toInt();
                int x = imageElement.attribute("topLeftX").toInt();
                int y = imageElement.attribute("topLeftY").toInt();
                qreal opacity = imageElement.attribute("opacity", "1.0").toDouble();
                loadImageAtFrame(path, QPoint(x, y), position, opacity);
            }

            progressStep();
        }
        imageTag = imageTag.nextSibling();
    }
}
