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
#include "layervector.h"

#include "vectorimage.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>


LayerVector::LayerVector(Object* object) : Layer(object, Layer::VECTOR)
{
    setName(tr("Vector Layer"));
}

LayerVector::~LayerVector()
{
}

bool LayerVector::usesColor(int colorIndex)
{
    bool bUseColor = false;
    foreachKeyFrame([&](KeyFrame* pKeyFrame)
    {
        auto pVecImage = static_cast<VectorImage*>(pKeyFrame);

        bUseColor = bUseColor || pVecImage->usesColor(colorIndex);
    });

    return bUseColor;
}

void LayerVector::removeColor(int colorIndex)
{
    foreachKeyFrame([=](KeyFrame* pKeyFrame)
    {
        auto pVecImage = static_cast<VectorImage*>(pKeyFrame);
        pVecImage->removeColor(colorIndex);
    });
}

void LayerVector::moveColor(int start, int end)
{
    foreachKeyFrame( [=] (KeyFrame* pKeyFrame)
    {
        auto pVecImage = static_cast<VectorImage*>(pKeyFrame);
        pVecImage->moveColor(start, end);
    });
}

void LayerVector::loadImageAtFrame(QString path, int frameNumber)
{
    if (keyExists(frameNumber))
    {
        removeKeyFrame(frameNumber);
    }
    VectorImage* vecImg = new VectorImage;
    vecImg->setPos(frameNumber);
    vecImg->setObject(object());
    vecImg->read(path);
    addKeyFrame(frameNumber, vecImg);
}

Status LayerVector::saveKeyFrameFile(KeyFrame* keyFrame, QString path)
{
    QString theFileName = fileName(keyFrame);
    QString strFilePath = QDir(path).filePath(theFileName);

    VectorImage* vecImage = static_cast<VectorImage*>(keyFrame);

    if (needSaveFrame(keyFrame, strFilePath) == false)
    {
        return Status::SAFE;
    }

    Status st = vecImage->write(strFilePath, "VEC");
    if (!st.ok())
    {
        vecImage->setFileName("");

        DebugDetails dd;
        dd << __FUNCTION__;
        dd << QString("KeyFrame.pos() = %1").arg(keyFrame->pos());
        dd << QString("FilePath = ").append(strFilePath);
        dd << "- VectorImage failed to write";
        dd.collect(st.details());
        return Status(Status::FAIL, dd);
    }

    vecImage->setFileName(strFilePath);
    vecImage->setModified(false);
    return Status::OK;
}

KeyFrame* LayerVector::createKeyFrame(int position, Object* obj)
{
    VectorImage* v = new VectorImage;
    v->setPos(position);
    v->setObject(obj);
    return v;
}

QString LayerVector::fileName(KeyFrame* key) const
{
    return QString::asprintf("%03d.%03d.vec", id(), key->pos());
}

bool LayerVector::needSaveFrame(KeyFrame* key, const QString& strSavePath)
{
    if (key->isModified()) // keyframe was modified
        return true;
    if (QFile::exists(strSavePath) == false) // hasn't been saved before
        return true;
    if (strSavePath != key->fileName()) // key frame moved
        return true;
    return false;
}

QDomElement LayerVector::createDomElement(QDomDocument& doc) const
{
    QDomElement layerElem = createBaseDomElement(doc);

    foreachKeyFrame([&](KeyFrame* keyframe)
    {
        QDomElement imageTag = doc.createElement("image");
        imageTag.setAttribute("frame", keyframe->pos());
        imageTag.setAttribute("src", fileName(keyframe));
        VectorImage* image = getVectorImageAtFrame(keyframe->pos());
        imageTag.setAttribute("opacity", image->getOpacity());
        layerElem.appendChild(imageTag);

        Q_ASSERT(QFileInfo(keyframe->fileName()).fileName() == fileName(keyframe));
    });

    return layerElem;
}

void LayerVector::loadDomElement(const QDomElement& element, QString dataDirPath, ProgressCallback progressStep)
{
    this->loadBaseDomElement(element);

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "image")
            {
                int position;
                if (!imageElement.attribute("src").isNull())
                {
                    QString path = dataDirPath + "/" + imageElement.attribute("src"); // the file is supposed to be in the data directory
                    QFileInfo fi(path);
                    if (!fi.exists()) path = imageElement.attribute("src");
                    position = imageElement.attribute("frame").toInt();
                    loadImageAtFrame(path, position);
                }
                else
                {
                    position = imageElement.attribute("frame").toInt();
                    addNewKeyFrameAt(position);
                    getVectorImageAtFrame(position)->loadDomElement(imageElement);
                }
                if (imageElement.hasAttribute("opacity"))
                    getVectorImageAtFrame(position)->setOpacity(imageElement.attribute("opacity").toDouble());
                else
                    getVectorImageAtFrame(position)->setOpacity(1.0);
                progressStep();
            }
        }
        imageTag = imageTag.nextSibling();
    }
}

VectorImage* LayerVector::getVectorImageAtFrame(int frameNumber) const
{
    return static_cast<VectorImage*>(getKeyFrameAt(frameNumber));
}

VectorImage* LayerVector::getLastVectorImageAtFrame(int frameNumber, int increment) const
{
    return static_cast<VectorImage*>(getLastKeyFrameAtPosition(frameNumber + increment));
}
