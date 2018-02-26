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
#include "layervector.h"

#include "vectorimage.h"


LayerVector::LayerVector(Object* object) : Layer(object, Layer::VECTOR)
{
    setName(tr("Vector Layer"));
}

LayerVector::~LayerVector()
{
}

bool LayerVector::usesColour(int colorIndex)
{
    bool bUseColor = false;
    foreachKeyFrame([&](KeyFrame* pKeyFrame)
    {
        auto pVecImage = static_cast<VectorImage*>(pKeyFrame);

        bUseColor = bUseColor || pVecImage->usesColour(colorIndex);
    });

    return bUseColor;
}

void LayerVector::removeColour(int colorIndex)
{
    foreachKeyFrame([=](KeyFrame* pKeyFrame)
    {
        auto pVecImage = static_cast<VectorImage*>(pKeyFrame);
        pVecImage->removeColour(colorIndex);
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

    qDebug() << "write: " << strFilePath;

    Status st = vecImage->write(strFilePath, "VEC");
    if (!st.ok())
    {
        QStringList debugInfo;
        debugInfo << "LayerVector::saveKeyFrame";
        debugInfo << QString("pKeyFrame.pos() = %1").arg(keyFrame->pos());
        debugInfo << QString("path = ").append(path);
        debugInfo << QString("strFilePath = ").append(strFilePath);

        QStringList vecImageDetails = st.detailsList();
        for (QString detail : vecImageDetails)
        {
            detail.prepend("&nbsp;&nbsp;");
        }
        debugInfo << QString("- VectorImage failed to write") << vecImageDetails;
        return Status(Status::FAIL, debugInfo);
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

QString LayerVector::fileName(KeyFrame* key)
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

QDomElement LayerVector::createDomElement(QDomDocument& doc)
{
    QDomElement layerTag = doc.createElement("layer");

    layerTag.setAttribute("id", id());
    layerTag.setAttribute("name", name());
    layerTag.setAttribute("visibility", visible());
    layerTag.setAttribute("type", type());

    foreachKeyFrame([&](KeyFrame* keyframe)
    {
        QDomElement imageTag = doc.createElement("image");
        imageTag.setAttribute("frame", keyframe->pos());
        imageTag.setAttribute("src", fileName(keyframe));
        layerTag.appendChild(imageTag);

        Q_ASSERT(QFileInfo(keyframe->fileName()).fileName() == fileName(keyframe));
    });

    return layerTag;
}

void LayerVector::loadDomElement(QDomElement element, QString dataDirPath, ProgressCallback progressStep)
{
    if (!element.attribute("id").isNull())
    {
        int id = element.attribute("id").toInt();
        setId(id);
    }
    setName(element.attribute("name"));
    setVisible(element.attribute("visibility") == "1");

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "image")
            {
                if (!imageElement.attribute("src").isNull())
                {
                    QString path = dataDirPath + "/" + imageElement.attribute("src"); // the file is supposed to be in the data directory
                    QFileInfo fi(path);
                    if (!fi.exists()) path = imageElement.attribute("src");
                    int position = imageElement.attribute("frame").toInt();
                    loadImageAtFrame(path, position);
                }
                else
                {
                    int frame = imageElement.attribute("frame").toInt();
                    addNewKeyFrameAt(frame);
                    getVectorImageAtFrame(frame)->loadDomElement(imageElement);
                }
                progressStep();
            }
        }
        imageTag = imageTag.nextSibling();
    }
}

VectorImage* LayerVector::getVectorImageAtFrame(int frameNumber)
{
    return static_cast<VectorImage*>(getKeyFrameAt(frameNumber));
}

VectorImage* LayerVector::getLastVectorImageAtFrame(int frameNumber, int increment)
{
    return static_cast<VectorImage*>(getLastKeyFrameAtPosition(frameNumber + increment));
}

