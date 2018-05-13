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

void LayerBitmap::loadImageAtFrame(QString path, QPoint topLeft, int frameNumber)
{
    BitmapImage* pKeyFrame = new BitmapImage(path, topLeft);
    pKeyFrame->setPos(frameNumber);
    loadKey(pKeyFrame);
}

Status LayerBitmap::saveKeyFrameFile(KeyFrame* keyframe, QString path)
{
    QString theFileName = fileName(keyframe);
    QString strFilePath = QDir(path).filePath(theFileName);

    if (needSaveFrame(keyframe, strFilePath) == false)
    {
        return Status::SAFE;
    }
    //qDebug() << "write: " << strFilePath;

    BitmapImage* bitmapImage = static_cast<BitmapImage*>(keyframe);
    Status st = bitmapImage->writeFile(strFilePath);
    if (!st.ok())
    {
        QStringList debugInfo;
        debugInfo << "LayerBitmap::saveKeyFrame"
            << QString("  KeyFrame.pos() = %1").arg(keyframe->pos())
            << QString("  path = %1").arg(path)
            << QString("  strFilePath = %1").arg(strFilePath)
            << QString("BitmapImage could not be saved");
        return Status(Status::FAIL, debugInfo);
    }

    bitmapImage->setFileName(strFilePath);
    bitmapImage->setModified(false);
    return Status::OK;
}

KeyFrame* LayerBitmap::createKeyFrame(int position, Object*)
{
    BitmapImage* b = new BitmapImage;
    b->setPos(position);
    return b;
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
    if (strSavePath != key->fileName()) // key frame moved
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
