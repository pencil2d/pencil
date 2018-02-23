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
#include "object.h"

#include <QDomDocument>
#include <QTextStream>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>

#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "layercamera.h"

#include "util.h"
#include "editor.h"
#include "bitmapimage.h"
#include "vectorimage.h"
#include "fileformat.h"

Object::Object(QObject* parent) : QObject(parent)
{
    setData(new ObjectData());
}

Object::~Object()
{
    while (!mLayers.empty())
    {
        delete mLayers.takeLast();
    }

    // Delete the working directory if this is not a "New" project.
    if (!filePath().isEmpty())
    {
        deleteWorkingDir();
    }
}

void Object::init()
{
    mData.reset(new ObjectData);

    createWorkingDir();

    // default palette
    loadDefaultPalette();
}

QDomElement Object::saveXML(QDomDocument& doc)
{
    QDomElement objectTag = doc.createElement("object");

    for (int i = 0; i < getLayerCount(); i++)
    {
        Layer* layer = getLayer(i);
        QDomElement layerTag = layer->createDomElement(doc);
        objectTag.appendChild(layerTag);
    }
    return objectTag;
}

bool Object::loadXML(QDomElement docElem, ProgressCallback progressForward)
{
    if (docElem.isNull())
    {
        return false;
    }
    int layerNumber = -1;

    const QString dataDirPath = mDataDirPath;

    for (QDomNode node = docElem.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if (element.tagName() == "layer")
        {
            switch (element.attribute("type").toInt() )
            {
            case Layer::BITMAP: addNewBitmapLayer(); break;
            case Layer::VECTOR: addNewVectorLayer(); break;
            case Layer::SOUND:  addNewSoundLayer();  break;
            case Layer::CAMERA: addNewCameraLayer(); break;
            default: Q_ASSERT(false); break;
            }
            layerNumber++;
            getLayer(layerNumber)->loadDomElement(element, dataDirPath, progressForward);
        }
    }
    return true;
}

LayerBitmap* Object::addNewBitmapLayer()
{
    LayerBitmap* layerBitmap = new LayerBitmap(this);
    mLayers.append(layerBitmap);

    layerBitmap->addNewKeyFrameAt(1);

    return layerBitmap;
}

LayerVector* Object::addNewVectorLayer()
{
    LayerVector* layerVector = new LayerVector(this);
    mLayers.append(layerVector);

    layerVector->addNewKeyFrameAt(1);

    return layerVector;
}

LayerSound* Object::addNewSoundLayer()
{
    LayerSound* layerSound = new LayerSound(this);
    mLayers.append(layerSound);

    // No default keyFrame at position 1 for Sound layer.

    return layerSound;
}

LayerCamera* Object::addNewCameraLayer()
{
    LayerCamera* layerCamera = new LayerCamera(this);
    mLayers.append(layerCamera);

    layerCamera->addNewKeyFrameAt(1);

    return layerCamera;
}

void Object::createWorkingDir()
{
    QString strFolderName;
    if (mFilePath.isEmpty())
    {
        strFolderName = "Default";
    }
    else
    {
        QFileInfo fileInfo(mFilePath);
        strFolderName = fileInfo.completeBaseName();
    }
    QString strWorkingDir = QDir::tempPath()
        + "/Pencil2D/"
        + strFolderName
        + PFF_TMP_DECOMPRESS_EXT
        + "/";

    QDir dir(QDir::tempPath());
    dir.mkpath(strWorkingDir);

    mWorkingDirPath = strWorkingDir;

    QDir dataDir(strWorkingDir + PFF_DATA_DIR);
    dataDir.mkpath(".");

    mDataDirPath = dataDir.absolutePath();
}

void Object::deleteWorkingDir() const
{
    QDir dir(mWorkingDirPath);
    dir.removeRecursively();
}

void Object::createDefaultLayers()
{
    // default layers
    addNewCameraLayer();
    addNewVectorLayer();
    addNewBitmapLayer();
}

int Object::getMaxLayerID()
{
    int maxId = 0;
    for (Layer* iLayer : mLayers)
    {
        if (iLayer->id() > maxId)
        {
            maxId = iLayer->id();
        }
    }
    return maxId;
}

int Object::getUniqueLayerID()
{
    return 1 + getMaxLayerID();
}

Layer* Object::getLayer(int i) const
{
    if (i < 0 || i >= getLayerCount())
    {
        return nullptr;
    }

    return mLayers.at(i);
}

Layer* Object::findLayerByName(QString strName, Layer::LAYER_TYPE type) const
{
    bool bCheckType = (type != Layer::UNDEFINED);
    for (Layer* layer : mLayers)
    {
        bool isTypeMatch = (bCheckType) ? (type == layer->type()) : true;
        if (isTypeMatch && layer->name() == strName)
        {
            return layer;
        }
    }
    return nullptr;
}

bool Object::moveLayer(int i, int j)
{
    if (i < 0 || i >= mLayers.size())
    {
        return false;
    }

    if (j < 0 || j >= mLayers.size())
    {
        return false;
    }

    if (i != j)
    {
        Layer* tmp = mLayers.at(i);
        mLayers[i] = mLayers.at(j);
        mLayers[j] = tmp;
    }
    return true;
}

void Object::deleteLayer(int i)
{
    if (i > -1 && i < mLayers.size())
    {
        disconnect(mLayers[i], 0, 0, 0); // disconnect the layer from this object
        delete mLayers.takeAt(i);
    }
}

void Object::deleteLayer(Layer* layer)
{
    auto it = std::find(mLayers.begin(), mLayers.end(), layer);

    if (it != mLayers.end())
    {
        disconnect(layer, 0, 0, 0);
        delete layer;
        mLayers.erase(it);
    }
}

ColourRef Object::getColour(int i)
{
    ColourRef result(Qt::white, "error");
    if (i > -1 && i < mPalette.size())
    {
        result = mPalette.at(i);
    }
    return result;
}

void Object::addColour(QColor colour)
{
    addColour(ColourRef(colour, "Colour " + QString::number(mPalette.size())));
}

bool Object::removeColour(int index)
{
    for (int i = 0; i < getLayerCount(); i++)
    {
        Layer* layer = getLayer(i);
        if (layer->type() == Layer::VECTOR)
        {
            LayerVector* layerVector = ((LayerVector*)layer);
            if (layerVector->usesColour(index)) return false;
        }
    }
    for (int i = 0; i < getLayerCount(); i++)
    {
        Layer* layer = getLayer(i);
        if (layer->type() == Layer::VECTOR)
        {
            LayerVector* layerVector = ((LayerVector*)layer);
            layerVector->removeColour(index);
        }
    }
    mPalette.removeAt(index);
    return true;
    // update the vector pictures using that colour !
}

void Object::renameColour(int i, QString text)
{
    mPalette[i].name = text;
}

bool Object::savePalette(QString filePath)
{
    return exportPalette(filePath + "/palette.xml");
}

bool Object::exportPalette(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug("Error: cannot export palette");
        return false;
    }
    QTextStream out(&file);

    QDomDocument doc("PencilPalette");
    QDomElement root = doc.createElement("palette");
    doc.appendChild(root);
    for (int i = 0; i < mPalette.size(); i++)
    {
        ColourRef ref = mPalette.at(i);
        QDomElement tag = doc.createElement("Colour");
        tag.setAttribute("name", ref.name);
        tag.setAttribute("red", ref.colour.red());
        tag.setAttribute("green", ref.colour.green());
        tag.setAttribute("blue", ref.colour.blue());
        tag.setAttribute("alpha", ref.colour.alpha());
        root.appendChild(tag);
    }

    int IndentSize = 2;
    doc.save(out, IndentSize);

    file.close();
    return true;
}

bool Object::importPalette(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
    {
        return false;
    }

    QDomDocument doc;
    doc.setContent(&file);

    mPalette.clear();
    QDomElement docElem = doc.documentElement();
    QDomNode tag = docElem.firstChild();
    while (!tag.isNull())
    {
        QDomElement e = tag.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            QString name = e.attribute("name");
            int r = e.attribute("red").toInt();
            int g = e.attribute("green").toInt();
            int b = e.attribute("blue").toInt();
            int a = e.attribute("alpha", "255").toInt();
            mPalette.append(ColourRef(QColor(r, g, b, a), name));
        }
        tag = tag.nextSibling();
    }
    file.close();
    return true;
}


void Object::loadDefaultPalette()
{
    mPalette.clear();
    addColour(ColourRef(QColor(Qt::black), QString(tr("Black"))));
    addColour(ColourRef(QColor(Qt::red), QString(tr("Red"))));
    addColour(ColourRef(QColor(Qt::darkRed), QString(tr("Dark Red"))));
    addColour(ColourRef(QColor(255, 128, 0), QString(tr("Orange"))));
    addColour(ColourRef(QColor(128, 64, 0), QString(tr("Dark Orange"))));
    addColour(ColourRef(QColor(Qt::yellow), QString(tr("Yellow"))));
    addColour(ColourRef(QColor(Qt::darkYellow), QString(tr("Dark Yellow"))));
    addColour(ColourRef(QColor(Qt::green), QString(tr("Green"))));
    addColour(ColourRef(QColor(Qt::darkGreen), QString(tr("Dark Green"))));
    addColour(ColourRef(QColor(Qt::cyan), QString(tr("Cyan"))));
    addColour(ColourRef(QColor(Qt::darkCyan), QString(tr("Dark Cyan"))));
    addColour(ColourRef(QColor(Qt::blue), QString(tr("Blue"))));
    addColour(ColourRef(QColor(Qt::darkBlue), QString(tr("Dark Blue"))));
    addColour(ColourRef(QColor(255, 255, 255), QString(tr("White"))));
    addColour(ColourRef(QColor(220, 220, 229), QString(tr("Very Light Grey"))));
    addColour(ColourRef(QColor(Qt::lightGray), QString(tr("Light Grey"))));
    addColour(ColourRef(QColor(Qt::gray), QString(tr("Grey"))));
    addColour(ColourRef(QColor(Qt::darkGray), QString(tr("Dark Grey"))));
    addColour(ColourRef(QColor(255, 227, 187), QString(tr("Light Skin"))));
    addColour(ColourRef(QColor(221, 196, 161), QString(tr("Light Skin - shade"))));
    addColour(ColourRef(QColor(255, 214, 156), QString(tr("Skin"))));
    addColour(ColourRef(QColor(207, 174, 127), QString(tr("Skin - shade"))));
    addColour(ColourRef(QColor(255, 198, 116), QString(tr("Dark Skin"))));
    addColour(ColourRef(QColor(227, 177, 105), QString(tr("Dark Skin - shade"))));
}

void Object::paintImage(QPainter& painter, int frameNumber,
                        bool background,
                        bool antialiasing) const
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // paints the background
    if (background)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.setWorldMatrixEnabled(false);
        painter.drawRect(QRect(0, 0, painter.device()->width(), painter.device()->height()));
        painter.setWorldMatrixEnabled(true);
    }

    for (int i = 0; i < getLayerCount(); i++)
    {
        Layer* layer = getLayer(i);
        if (layer->visible())
        {
            painter.setOpacity(1.0);

            // paints the bitmap images
            if (layer->type() == Layer::BITMAP)
            {
                LayerBitmap* layerBitmap = (LayerBitmap*)layer;
                layerBitmap->getLastBitmapImageAtFrame(frameNumber, 0)->paintImage(painter);
            }
            // paints the vector images
            if (layer->type() == Layer::VECTOR)
            {
                LayerVector* layerVector = (LayerVector*)layer;
                layerVector->getLastVectorImageAtFrame(frameNumber, 0)->paintImage(painter,
                                                                                   false,
                                                                                   false,
                                                                                   antialiasing);
            }
        }
    }
}

QString Object::copyFileToDataFolder(QString strFilePath)
{
    if (!QFile::exists(strFilePath))
    {
        qDebug() << "[Object] sound file doesn't exist: " << strFilePath;
        return "";
    }

    QString sNewFileName = "sound_";
    sNewFileName += QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz.");
    sNewFileName += QFileInfo(strFilePath).suffix();

    QString srcFile = strFilePath;
    QString destFile = QDir(mDataDirPath).filePath(sNewFileName);

    if (QFile::exists(destFile))
    {
        QFile::remove(destFile);
    }

    bool bCopyOK = QFile::copy(srcFile, destFile);
    if (!bCopyOK)
    {
        qDebug() << "[Object] couldn't copy sound file to data folder: " << strFilePath;
        return "";
    }

    return destFile;
}

bool Object::exportFrames(int frameStart, int frameEnd,
                          LayerCamera* cameraLayer,
                          QSize exportSize,
                          QString filePath,
                          QString format,
                          bool transparency,
                          bool antialiasing,
                          QProgressDialog* progress = NULL,
                          int progressMax = 50)
{
    Q_ASSERT(cameraLayer);

    QSettings settings(PENCIL2D, PENCIL2D);

    QString extension = "";
    QString formatStr = format;
    if (formatStr == "PNG" || formatStr == "png")
    {
        format = "PNG";
        extension = ".png";
    }
    if (formatStr == "JPG" || formatStr == "jpg" || formatStr == "JPEG" || formatStr == "jpeg")
    {
        format = "JPG";
        extension = ".jpg";
        transparency = false; // JPG doesn't support transparency so we have to include the background
    }
    if (filePath.endsWith(extension, Qt::CaseInsensitive))
    {
        filePath.chop(extension.size());
    }

    qDebug() << "Exporting frames from "
        << frameStart << "to"
        << frameEnd
        << "at size " << exportSize;

    for (int currentFrame = frameStart; currentFrame <= frameEnd; currentFrame++)
    {
        if (progress != NULL)
        {
            int totalFramesToExport = (frameEnd - frameStart) + 1;
            if (totalFramesToExport != 0) // Avoid dividing by zero.
            {
                progress->setValue((currentFrame - frameStart + 1)*progressMax / totalFramesToExport);
                QApplication::processEvents();  // Required to make progress bar update on-screen.
            }

            if (progress->wasCanceled())
            {
                break;
            }
        }

        QTransform view = cameraLayer->getViewAtFrame(currentFrame);
        QSize camSize = cameraLayer->getViewSize();

        QString frameNumberString = QString::number(currentFrame);
        while (frameNumberString.length() < 4)
        {
            frameNumberString.prepend("0");
        }
        QString sFileName = filePath + frameNumberString + extension;

        exportIm(currentFrame, view, camSize, exportSize, sFileName, format, antialiasing, transparency);
    }

    return true;
}

bool Object::exportX(int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing)
{
    QSettings settings(PENCIL2D, PENCIL2D);

    int page;
    page = 0;
    for (int j = frameStart; j <= frameEnd; j = j + 15)
    {
        QImage xImg(QSize(2300, 3400), QImage::Format_ARGB32_Premultiplied);
        QPainter xPainter(&xImg);
        xPainter.fillRect(0, 0, 2300, 3400, Qt::white);
        int y = j - 1;
        for (int i = j; i < 15 + page * 15 && i <= frameEnd; i++)
        {
            QRect source = QRect(QPoint(0, 0), exportSize);
            QRect target = QRect(QPoint((y % 3) * 800 + 30, (y / 3) * 680 + 50 - page * 3400), QSize(640, 480));
            QTransform thumbView = view * RectMapTransform(source, target);
            xPainter.setWorldTransform(thumbView);
            xPainter.setClipRegion(thumbView.inverted().map(QRegion(target)));
            paintImage(xPainter, i, false, antialiasing);
            xPainter.resetMatrix();
            xPainter.setClipping(false);
            xPainter.setPen(Qt::black);
            xPainter.drawRect(target);
            xPainter.drawText(QPoint((y % 3) * 800 + 35, (y / 3) * 680 + 65 - page * 3400), QString::number(i));
            y++;
        }

        if (filePath.endsWith(".jpg", Qt::CaseInsensitive))
        {
            filePath.chop(4);
        }
        if (!xImg.save(filePath + QString::number(page) + ".jpg", "JPG", 60)) {
            return false;
        }
        page++;
    }

    return true;
}

bool Object::exportIm(int frame, QTransform view, QSize cameraSize, QSize exportSize, QString filePath, QString format, bool antialiasing, bool transparency)
{
    QImage imageToExport(exportSize, QImage::Format_ARGB32_Premultiplied);

    QColor bgColor = Qt::white;
    if (transparency)
        bgColor.setAlpha(0);
    imageToExport.fill(bgColor);

    QTransform centralizeCamera;
    centralizeCamera.translate(cameraSize.width() / 2, cameraSize.height() / 2);

    QPainter painter(&imageToExport);
    painter.setWorldTransform(view * centralizeCamera);
    painter.setWindow(QRect(0, 0, cameraSize.width(), cameraSize.height()));

    paintImage(painter, frame, false, antialiasing);

    return imageToExport.save(filePath, format.toStdString().c_str());
}

int Object::getLayerCount() const
{
    return mLayers.size();
}

ObjectData* Object::data()
{
    Q_ASSERT(mData != nullptr);
    return mData.get();
}

void Object::setData(ObjectData* d)
{
    Q_ASSERT(d != nullptr);
    mData.reset(d);
}

int Object::totalKeyFrameCount()
{
    int sum = 0;
    for (Layer* layer : mLayers)
    {
        sum += layer->keyFrameCount();
    }
    return sum;
}
