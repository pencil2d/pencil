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
#include <QProgressDialog>
#include <QApplication>
#include <QFile>

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
#include "activeframepool.h"


Object::Object(QObject* parent) : QObject(parent)
{
    setData(new ObjectData());
    mActiveFramePool.reset(new ActiveFramePool(400));
}

Object::~Object()
{
    mActiveFramePool->clear();

    for (Layer* layer : mLayers)
        delete layer;
    mLayers.clear();

    deleteWorkingDir();
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
            switch (element.attribute("type").toInt())
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

    connect(layerCamera, &LayerCamera::resolutionChanged, this, &Object::layerViewChanged);

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
    QDir dir(QDir::tempPath());

    QString strWorkingDir;
    do
    {
        strWorkingDir = QString("%1/Pencil2D/%2_%3_%4/")
            .arg(QDir::tempPath())
            .arg(strFolderName)
            .arg(PFF_TMP_DECOMPRESS_EXT)
            .arg(uniqueString(8));
    }
    while(dir.exists(strWorkingDir));

    dir.mkpath(strWorkingDir);
    mWorkingDirPath = strWorkingDir;

    QDir dataDir(strWorkingDir + PFF_DATA_DIR);
    dataDir.mkpath(".");

    mDataDirPath = dataDir.absolutePath();
}

void Object::deleteWorkingDir() const
{
    if (!mWorkingDirPath.isEmpty())
    {
        QDir dir(mWorkingDirPath);
        dir.removeRecursively();
    }
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

bool Object::swapLayers(int i, int j)
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
        delete mLayers.takeAt(i);
    }
}

void Object::deleteLayer(Layer* layer)
{
    auto it = std::find(mLayers.begin(), mLayers.end(), layer);

    if (it != mLayers.end())
    {
        delete layer;
        mLayers.erase(it);
    }
}

ColourRef Object::getColour(int index) const
{
    ColourRef result(Qt::white, "error");
    if (index > -1 && index < mPalette.size())
    {
        result = mPalette.at(index);
    }
    return result;
}

void Object::setColour(int index, QColor newColour)
{
    Q_ASSERT(index >= 0);

    mPalette[index].colour = newColour;
}

void Object::setColourRef(int index, ColourRef newColourRef)
{
    mPalette[index] = newColourRef;
}

void Object::addColour(QColor colour)
{
    addColour(ColourRef(colour, "Colour " + QString::number(mPalette.size())));
}

void Object::addColourAtIndex(int index, ColourRef newColour)
{
    mPalette.insert(index, newColour);
}

bool Object::isColourInUse(int index)
{
    for (int i = 0; i < getLayerCount(); i++)
    {
        Layer* layer = getLayer(i);
        if (layer->type() == Layer::VECTOR)
        {
            LayerVector* layerVector = static_cast<LayerVector*>(layer);

            if (layerVector->usesColour(index))
            {
                return true;
            }
        }
    }
    return false;

}

void Object::removeColour(int index)
{
    for (int i = 0; i < getLayerCount(); i++)
    {
        Layer* layer = getLayer(i);
        if (layer->type() == Layer::VECTOR)
        {
            LayerVector* layerVector = static_cast<LayerVector*>(layer);
            layerVector->removeColour(index);
        }
    }

    mPalette.removeAt(index);

    // update the vector pictures using that colour !
}

void Object::renameColour(int i, QString text)
{
    mPalette[i].name = text;
}

QString Object::savePalette(QString dataFolder)
{
    QString fullPath = QDir(dataFolder).filePath("palette.xml");
    bool ok = exportPalette(fullPath);
    if (ok)
        return fullPath;
    return "";
}

void Object::exportPaletteGPL(QFile& file)
{

    QString fileName = QFileInfo(file).baseName();
    QTextStream out(&file);

    out << "GIMP Palette" << "\n";
    out << "Name: " << fileName << "\n";
    out << "#" << "\n";

    for (ColourRef ref : mPalette)
    {
        QColor toRgb = ref.colour.toRgb();
        out << QString("%1 %2 %3").arg(toRgb.red()).arg(toRgb.green()).arg(toRgb.blue());
        out << " " << ref.name << "\n";
    }
}

void Object::exportPalettePencil(QFile& file)
{
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
}

bool Object::exportPalette(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug("Error: cannot export palette");
        return false;
    }

    if (file.fileName().endsWith(".gpl", Qt::CaseInsensitive))
    {
        exportPaletteGPL(file);
    } else {
        exportPalettePencil(file);
    }

    file.close();
    return true;
}

/* Import the .gpl GIMP palette format.
 *
 * This functions supports importing both the old and new .gpl formats.
 * This should load colors the same as GIMP, with the following intentional exceptions:
 * - Whitespace before and after a name does not appear in the name
 * - The last line is processed, even if there is not a trailing newline
 * - Colours without a name will use our automatic naming system rather than "Untitled"
 */
void Object::importPaletteGPL(QFile& file)
{
    QTextStream in(&file);
    QString line;

    // First line must start with "GIMP Palette"
    // Displaying an error here would be nice
    in.readLineInto(&line);
    if (!line.startsWith("GIMP Palette")) return;

    in.readLineInto(&line);

    // There are two GPL formats, the new one must start with "Name: " on the second line
    if (line.startsWith("Name: "))
    {
        in.readLineInto(&line);
        // The new format contains an optional thrid line starting with "Columns: "
        if (line.startsWith("Columns: "))
        {
            // Skip to next line
            in.readLineInto(&line);
        }
    }

    // Colours inherit the value from the previous colour for missing channels
    // Some palettes may rely on this behavior so we should try to replicate it
    QColor prevColour(Qt::black);

    do
    {
        // Ignore comments and empty lines
        if (line.isEmpty() || line.startsWith("#")) continue;

        int red = 0;
        int green = 0;
        int blue = 0;

        int countInLine = 0;
        QString name = "";

        for(const QString& snip : line.split(QRegExp("\\s|\\t"), QString::SkipEmptyParts))
        {
            switch (countInLine)
            {
            case 0:
                red = snip.toInt();
                break;
            case 1:
                green = snip.toInt();
                break;
            case 2:
                blue = snip.toInt();
                break;
            default:
                name += snip + " ";
            }
            countInLine++;
        }

        // trim additional spaces
        name = name.trimmed();

        // Get values from previous colour if necessary
        if (countInLine < 2) green = prevColour.green();
        if (countInLine < 3) blue = prevColour.blue();

        // GIMP assigns colours the name "Untitled" by default now
        // so in addition to missing names, we also use automatic
        // naming for this
        if (name.isEmpty() || name == "Untitled") name = QString();

        QColor colour(red, green, blue);
        if (colour.isValid())
        {
            mPalette.append(ColourRef(colour, name));
            prevColour = colour;
        }
    } while (in.readLineInto(&line));
}

void Object::importPalettePencil(QFile& file)
{
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
}

bool Object::importPalette(QString filePath)
{
    QFile file(filePath);

    if (!file.open(QFile::ReadOnly))
    {
        return false;
    }

    if (file.fileName().endsWith(".gpl", Qt::CaseInsensitive))
    {
        importPaletteGPL(file);
    } else {
        importPalettePencil(file);
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
    addColour(ColourRef(QColor(227, 177, 105), QString(tr("Dark Skin - shade")) ));
}

void Object::paintImage(QPainter& painter,int frameNumber,
                        bool background,
                        bool antialiasing) const
{
    updateActiveFrames(frameNumber);

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
                LayerBitmap* layerBitmap = static_cast<LayerBitmap*>(layer);

                BitmapImage* bitmap = layerBitmap->getLastBitmapImageAtFrame(frameNumber);
                if (bitmap)
                    bitmap->paintImage(painter);

            }
            // paints the vector images
            if (layer->type() == Layer::VECTOR)
            {
                LayerVector* layerVector = static_cast<LayerVector*>(layer);
                VectorImage* vec = layerVector->getLastVectorImageAtFrame(frameNumber, 0);
                if (vec)
                    vec->paintImage(painter, false, false, antialiasing);
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
                          bool exportKeyframesOnly,
                          QString layerName,
                          bool antialiasing,
                          QProgressDialog* progress = nullptr,
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
    if (formatStr == "TIFF" || formatStr == "tiff" || formatStr == "TIF" || formatStr == "tif")
    {
        format = "TIFF";
        extension = ".tiff";
    }
    if (formatStr == "BMP" || formatStr == "bmp")
    {
        format = "BMP";
        extension = ".bmp";
        transparency = false;
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
        if (progress != nullptr)
        {
            int totalFramesToExport = (frameEnd - frameStart) + 1;
            if (totalFramesToExport != 0) // Avoid dividing by zero.
            {
                progress->setValue((currentFrame - frameStart + 1) * progressMax / totalFramesToExport);
                QApplication::processEvents(); // Required to make progress bar update on-screen.
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
        Layer* layer = findLayerByName(layerName);
        if (exportKeyframesOnly)
        {
            if (layer->keyExists(currentFrame))
                exportIm(currentFrame, view, camSize, exportSize, sFileName, format, antialiasing, transparency);
        }
        else
        {
            exportIm(currentFrame, view, camSize, exportSize, sFileName, format, antialiasing, transparency);
        }
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
        if (!xImg.save(filePath + QString::number(page) + ".jpg", "JPG", 60))
        {
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

void Object::updateActiveFrames(int frame) const
{
    int beginFrame = std::max(frame - 3, 1);
    int endFrame = frame + 4;
    for (int i = 0; i < getLayerCount(); ++i)
    {
        Layer* layer = getLayer(i);
        for (int k = beginFrame; k < endFrame; ++k)
        {
            KeyFrame* key = layer->getKeyFrameAt(k);
            mActiveFramePool->put(key);
        }
    }
}

void Object::setActiveFramePoolSize(int n)
{
    mActiveFramePool->resize(n);
}
