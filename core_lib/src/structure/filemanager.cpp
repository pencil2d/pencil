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

#include "filemanager.h"

#include <ctime>
#include <QDir>
#include "pencildef.h"
#include "qminiz.h"
#include "fileformat.h"
#include "object.h"
#include "layercamera.h"

namespace
{
    QString openErrorTitle = QObject::tr("Could not open file");
    QString openErrorDesc = QObject::tr("There was an error processing your file. This usually means that your project has "
                             "been at least partially corrupted. You can try again with a newer version of Pencil2D, "
                             "or you can try to use a backup file if you have one. If you contact us through one of "
                             "our official channels we may be able to help you. For reporting issues, "
                             "the best places to reach us are:");
    QString contactLinks = "<ul>"
                           "<li><a href=\"https://discuss.pencil2d.org/c/bugs\">Pencil2D Forum</a></li>"
                           "<li><a href=\"https://github.com/pencil2d/pencil/issues/new\">Github</a></li>"
                           "<li><a href=\"https://discord.gg/8FxdV2g\">Discord<\a></li>"
                           "</ul>";
}

FileManager::FileManager(QObject *parent) : QObject(parent),
mLog("FileManager")
{
    ENABLE_DEBUG_LOG(mLog, false);
    srand(static_cast<uint>(time(nullptr)));
}

Object* FileManager::load(QString sFileName)
{
    DebugDetails dd;
    dd << QString("File name: ").append(sFileName);
    if (!QFile::exists(sFileName))
    {
        qCDebug(mLog) << "ERROR - File doesn't exist.";
        return cleanUpWithErrorCode(Status(Status::FILE_NOT_FOUND, dd, tr("Could not open file"),
                                           tr("The file does not exist, so we are unable to open it. Please check "
                                           "to make sure the path is correct and that the file is accessible and try again.")));
    }

    progressForward();

    Object* obj = new Object;
    obj->setFilePath(sFileName);
    obj->createWorkingDir();

    QString strMainXMLFile;
    QString strDataFolder;

    // Test file format: new zipped .pclx or old .pcl?
    bool oldFormat = isOldForamt(sFileName);
    dd << QString("Is old format: ").append(oldFormat ? "true" : "false");

    if (oldFormat)
    {
        dd << "Recognized Old Pencil File Format (*.pcl) !";

        strMainXMLFile = sFileName;
        strDataFolder = strMainXMLFile + "." + PFF_OLD_DATA_DIR;
    }
    else
    {
        dd << "Recognized New zipped Pencil File Format (*.pclx) !";

        unzip(sFileName, obj->workingDir());

        strMainXMLFile = QDir(obj->workingDir()).filePath(PFF_XML_FILE_NAME);
        strDataFolder = QDir(obj->workingDir()).filePath(PFF_DATA_DIR);
    }

    dd << QString("XML file: ").append(strMainXMLFile)
       << QString("Data folder: ").append(strDataFolder)
       << QString("Working folder: ").append(obj->workingDir());

    obj->setDataDir(strDataFolder);
    obj->setMainXMLFile(strMainXMLFile);

    int totalFileCount = QDir(strDataFolder).entryList(QDir::Files).size();
    mMaxProgressValue = totalFileCount;
    emit progressRangeChanged(mMaxProgressValue);

    QFile file(strMainXMLFile);
    if (!file.exists())
    {
        dd << "Main XML file does not exist";
        return cleanUpWithErrorCode(Status(Status::ERROR_INVALID_XML_FILE, dd, openErrorTitle, openErrorDesc.append(contactLinks)));
    }
    if (!file.open(QFile::ReadOnly))
    {
        return cleanUpWithErrorCode(Status(Status::ERROR_FILE_CANNOT_OPEN, dd, tr("Could not open file"),
                                           tr("This program does not have permission to read the file you have selected. "
                                              "Please check that you have read permissions for this file and try again.")));
    }

    QDomDocument xmlDoc;
    if (!xmlDoc.setContent(&file))
    {
        qCDebug(mLog) << "Couldn't open the main XML file.";
        dd << "Error parsing or opening the main XML file";
        return cleanUpWithErrorCode(Status(Status::ERROR_INVALID_XML_FILE, dd, openErrorTitle, openErrorDesc.append(contactLinks)));
    }

    QDomDocumentType type = xmlDoc.doctype();
    if (!(type.name() == "PencilDocument" || type.name() == "MyObject"))
    {
        dd << QString("Invalid main XML doctype: ").append(type.name());
        return cleanUpWithErrorCode(Status(Status::ERROR_INVALID_PENCIL_FILE, dd, openErrorTitle, openErrorDesc.append(contactLinks)));
    }

    QDomElement root = xmlDoc.documentElement();
    if (root.isNull())
    {
        dd << "Main XML root node is null";
        return cleanUpWithErrorCode(Status(Status::ERROR_INVALID_PENCIL_FILE, dd, openErrorTitle, openErrorDesc.append(contactLinks)));
    }

    loadPalette(obj);

    bool ok = true;

    if (root.tagName() == "document")
    {
        ok = loadObject(obj, root);
    }
    else if (root.tagName() == "object" || root.tagName() == "MyOject") // old Pencil format (<=0.4.3)
    {
        ok = loadObjectOldWay(obj, root);
    }

    if (!ok)
    {
        delete obj;
        dd << "Issue occurred during object loading";
        return cleanUpWithErrorCode(Status(Status::ERROR_INVALID_PENCIL_FILE, dd, ""));
    }

    verifyObject(obj);

    return obj;
}

bool FileManager::loadObject(Object* object, const QDomElement& root)
{
    QDomElement e = root.firstChildElement("object");
    if (e.isNull())
        return false;

    bool ok = true;
    for (QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if (element.isNull())
        {
            continue;
        }

        if (element.tagName() == "object")
        {
            ok = object->loadXML(element, [this]{ progressForward(); });

            if (!ok) qCDebug(mLog) << "Failed to Load object";

        }
        else if (element.tagName() == "editor" || element.tagName() == "projectdata")
        {
            ObjectData* projectData = loadProjectData(element);
            object->setData(projectData);
        }
        else
        {
            Q_ASSERT(false);
        }
    }
    return ok;
}

bool FileManager::loadObjectOldWay(Object* object, const QDomElement& root)
{
    return object->loadXML(root, [this] { progressForward(); });
}

bool FileManager::isOldForamt(const QString& fileName) const
{
    return !(MiniZ::isZip(fileName));
}

Status FileManager::save(Object* object, QString sFileName)
{
    DebugDetails dd;
    dd << "FileManager::save";
    dd << ("sFileName = " + sFileName);

    if (object == nullptr)
    {
        dd << "object parameter is null";
        return Status(Status::INVALID_ARGUMENT, dd);
    }

    int totalCount = object->totalKeyFrameCount();
    mMaxProgressValue = totalCount + 5;
    emit progressRangeChanged(mMaxProgressValue);

    progressForward();

    QFileInfo fileInfo(sFileName);
    if (fileInfo.isDir())
    {
        dd << "FileName points to a directory";
        return Status(Status::INVALID_ARGUMENT, dd,
                      tr("Invalid Save Path"),
                      tr("The path (\"%1\") points to a directory.").arg(fileInfo.absoluteFilePath()));
    }
    QFileInfo parentDirInfo(fileInfo.dir().absolutePath());
    if (!parentDirInfo.exists())
    {
        dd << "The parent directory of sFileName does not exist";
        return Status(Status::INVALID_ARGUMENT, dd,
                      tr("Invalid Save Path"),
                      tr("The directory (\"%1\") does not exist.").arg(parentDirInfo.absoluteFilePath()));
    }
    if ((fileInfo.exists() && !fileInfo.isWritable()) || !parentDirInfo.isWritable())
    {
        dd << "Filename points to a location that is not writable";
        return Status(Status::INVALID_ARGUMENT, dd,
                      tr("Invalid Save Path"),
                      tr("The path (\"%1\") is not writable.").arg(fileInfo.absoluteFilePath()));
    }

    QString sTempWorkingFolder;
    QString sMainXMLFile;
    QString sDataFolder;

    bool isOldType = sFileName.endsWith(PFF_OLD_EXTENSION);
    if (isOldType)
    {
        dd << "Old Pencil File Format (*.pcl) !";

        sMainXMLFile = sFileName;
        sDataFolder = sMainXMLFile + "." + PFF_OLD_DATA_DIR;
    }
    else
    {
        dd << "New zipped Pencil File Format (*.pclx) !";

        sTempWorkingFolder = object->workingDir();
        Q_ASSERT(QDir(sTempWorkingFolder).exists());
        dd << QString("TempWorkingFolder = ").append(sTempWorkingFolder);

        sMainXMLFile = QDir(sTempWorkingFolder).filePath(PFF_XML_FILE_NAME);
        sDataFolder = QDir(sTempWorkingFolder).filePath(PFF_OLD_DATA_DIR);
    }

    QFileInfo dataInfo(sDataFolder);
    if (!dataInfo.exists())
    {
        QDir dir(sDataFolder); // the directory where all key frames will be saved

        if (!dir.mkpath(sDataFolder))
        {
            dd << QString("dir.absolutePath() = %1").arg(dir.absolutePath());
            return Status(Status::FAIL, dd,
                          tr("Cannot Create Data Directory"),
                          tr("Failed to create directory \"%1\". Please make sure you have sufficient permissions.").arg(sDataFolder));
        }
    }
    if (!dataInfo.isDir())
    {
        dd << QString("dataInfo.absoluteFilePath() = ").append(dataInfo.absoluteFilePath());
        return Status(Status::FAIL,
                      dd,
                      tr("Cannot Create Data Directory"),
                      tr("\"%1\" is a file. Please delete the file and try again.").arg(dataInfo.absoluteFilePath()));
    }

    // save data
    int numLayers = object->getLayerCount();
    dd << QString("Total %1 layers").arg(numLayers);

    for (int i = 0; i < numLayers; ++i)
    {
        Layer* layer = object->getLayer(i);
        layer->presave(sDataFolder);
    }

    QStringList zippedFiles;

    bool saveLayersOK = true;
    for (int i = 0; i < numLayers; ++i)
    {
        Layer* layer = object->getLayer(i);

        dd << QString("Layer[%1] = [id=%2, name=%3, type=%4]").arg(i).arg(layer->id()).arg(layer->name()).arg(layer->type());
        
        Status st = layer->save(sDataFolder, zippedFiles, [this] { progressForward(); });
        if (!st.ok())
        {
            saveLayersOK = false;
            dd.collect(st.details());
            dd << QString("  !! Failed to save Layer[%1] %2").arg(i).arg(layer->name());
        }
    }
    dd << "All Layers saved";

    // save palette
    QString sPaletteFile = object->savePalette(sDataFolder);
    if (!sPaletteFile.isEmpty())
        zippedFiles.append(sPaletteFile);
    else
        dd << "Failed to save the palette xml";
    
    progressForward();

    // -------- save main XML file -----------
    QFile file(sMainXMLFile);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        return Status(Status::ERROR_FILE_CANNOT_OPEN, dd);
    }

    QDomDocument xmlDoc("PencilDocument");
    QDomElement root = xmlDoc.createElement("document");
    QDomProcessingInstruction encoding = xmlDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    xmlDoc.appendChild(encoding);
    xmlDoc.appendChild(root);

    progressForward();

    // save editor information
    QDomElement projDataXml = saveProjectData(object->data(), xmlDoc);
    root.appendChild(projDataXml);

    // save object
    QDomElement objectElement = object->saveXML(xmlDoc);
    root.appendChild(objectElement);

    dd << "Writing main xml file...";

    const int indentSize = 2;

    QTextStream out(&file);
    xmlDoc.save(out, indentSize);
    out.flush();
    file.close();

    dd << "Done writing main xml file at" << sMainXMLFile;

    zippedFiles.append(sMainXMLFile);

    progressForward();

    if (!isOldType)
    {
        dd << "Miniz";

        QString sBackupFile = backupPreviousFile(sFileName);

        Status s = MiniZ::compressFolder(sFileName, sTempWorkingFolder, zippedFiles);
        if (!s.ok())
        {
            dd.collect(s.details());
            return Status(Status::ERROR_MINIZ_FAIL, dd, 
                          tr("Miniz Error"),
                          tr("An internal error occurred. Your file may not be saved successfully."));
        }
        dd << "Zip file saved successfully";

        if (s.ok() && saveLayersOK)
            deleteBackupFile(sBackupFile);
    }

    object->setFilePath(sFileName);
    object->setModified(false);

    progressForward();

    if (!saveLayersOK)
    {
        return Status(Status::FAIL, dd,
                      tr("Internal Error"),
                      tr("An internal error occurred. Your file may not be saved successfully."));
    }

    return Status::OK;
}

ObjectData* FileManager::loadProjectData(const QDomElement& docElem)
{
    ObjectData* data = new ObjectData;
    if (docElem.isNull())
    {
        return data;
    }

    QDomNode tag = docElem.firstChild();

    while (!tag.isNull())
    {
        QDomElement element = tag.toElement(); // try to convert the node to an element.
        if (element.isNull())
        {
            continue;
        }

        extractProjectData(element, data);

        tag = tag.nextSibling();
    }
    return data;
}

QDomElement FileManager::saveProjectData(ObjectData* data, QDomDocument& xmlDoc)
{
    QDomElement rootTag = xmlDoc.createElement("projectdata");

    // Current Frame
    QDomElement currentFrameTag = xmlDoc.createElement("currentFrame");
    currentFrameTag.setAttribute("value", data->getCurrentFrame());
    rootTag.appendChild(currentFrameTag);

    // Current Colour
    QDomElement currentColorTag = xmlDoc.createElement("currentColor");
    QColor color = data->getCurrentColor();
    currentColorTag.setAttribute("r", color.red());
    currentColorTag.setAttribute("g", color.green());
    currentColorTag.setAttribute("b", color.blue());
    currentColorTag.setAttribute("a", color.alpha());
    rootTag.appendChild(currentColorTag);

    // Current Layer
    QDomElement currentLayerTag = xmlDoc.createElement("currentLayer");
    currentLayerTag.setAttribute("value", data->getCurrentLayer());
    rootTag.appendChild(currentLayerTag);

    // Current View
    QDomElement currentViewTag = xmlDoc.createElement("currentView");
    QTransform view = data->getCurrentView();
    currentViewTag.setAttribute("m11", view.m11());
    currentViewTag.setAttribute("m12", view.m12());
    currentViewTag.setAttribute("m21", view.m21());
    currentViewTag.setAttribute("m22", view.m22());
    currentViewTag.setAttribute("dx", view.dx());
    currentViewTag.setAttribute("dy", view.dy());
    rootTag.appendChild(currentViewTag);

    // Fps
    QDomElement fpsTag = xmlDoc.createElement("fps");
    fpsTag.setAttribute("value", data->getFrameRate());
    rootTag.appendChild(fpsTag);

    // Current Layer
    QDomElement tagIsLoop = xmlDoc.createElement("isLoop");
    tagIsLoop.setAttribute("value", data->isLooping() ? "true" : "false");
    rootTag.appendChild(tagIsLoop);

    QDomElement tagRangedPlayback = xmlDoc.createElement("isRangedPlayback");
    tagRangedPlayback.setAttribute("value", data->isRangedPlayback() ? "true" : "false");
    rootTag.appendChild(tagRangedPlayback);

    QDomElement tagMarkInFrame = xmlDoc.createElement("markInFrame");
    tagMarkInFrame.setAttribute("value", data->getMarkInFrameNumber());
    rootTag.appendChild(tagMarkInFrame);

    QDomElement tagMarkOutFrame = xmlDoc.createElement("markOutFrame");
    tagMarkOutFrame.setAttribute("value", data->getMarkOutFrameNumber());
    rootTag.appendChild(tagMarkOutFrame);

    return rootTag;
}

void FileManager::extractProjectData(const QDomElement& element, ObjectData* data)
{
    Q_ASSERT(data);

    QString strName = element.tagName();
    if (strName == "currentFrame")
    {
        data->setCurrentFrame(element.attribute("value").toInt());
    }
    else  if (strName == "currentColor")
    {
        int r = element.attribute("r", "255").toInt();
        int g = element.attribute("g", "255").toInt();
        int b = element.attribute("b", "255").toInt();
        int a = element.attribute("a", "255").toInt();

        data->setCurrentColor(QColor(r, g, b, a));
    }
    else if (strName == "currentLayer")
    {
        data->setCurrentLayer(element.attribute("value", "0").toInt());
    }
    else if (strName == "currentView")
    {
        double m11 = element.attribute("m11", "1").toDouble();
        double m12 = element.attribute("m12", "0").toDouble();
        double m21 = element.attribute("m21", "0").toDouble();
        double m22 = element.attribute("m22", "1").toDouble();
        double dx = element.attribute("dx", "0").toDouble();
        double dy = element.attribute("dy", "0").toDouble();

        data->setCurrentView(QTransform(m11, m12, m21, m22, dx, dy));
    }
    else if (strName == "fps" || strName == "currentFps")
    {
        data->setFrameRate(element.attribute("value", "12").toInt());
    }
    else if (strName == "isLoop")
    {
        data->setLooping(element.attribute("value", "false") == "true");
    }
    else if (strName == "isRangedPlayback")
    {
        data->setRangedPlayback((element.attribute("value", "false") == "true"));
    }
    else if (strName == "markInFrame")
    {
        data->setMarkInFrameNumber(element.attribute("value", "0").toInt());
    }
    else if (strName == "markOutFrame")
    {
        data->setMarkOutFrameNumber(element.attribute("value", "15").toInt());
    }
}

Object* FileManager::cleanUpWithErrorCode(Status error)
{
    mError = error;
    removePFFTmpDirectory(mstrLastTempFolder);
    return nullptr;
}

QString FileManager::backupPreviousFile(const QString& fileName)
{
    if (!QFile::exists(fileName))
        return "";

    QFileInfo info(fileName);
    QString sBackupFile = info.completeBaseName() + ".backup." + info.suffix();
    QString sBackupFileFullPath = QDir(info.absolutePath()).filePath(sBackupFile);

    bool ok = QFile::rename(info.absoluteFilePath(), sBackupFileFullPath);
    if (!ok)
    {
        qDebug() << "Cannot backup the previous file.";
        return "";
    }
    return sBackupFileFullPath;
}

void FileManager::deleteBackupFile(const QString& fileName)
{
    if (QFile::exists(fileName))
    {
        QFile::remove(fileName);
    }
}

void FileManager::progressForward()
{
    mCurrentProgress++;
    emit progressChanged(mCurrentProgress);
}

bool FileManager::loadPalette(Object* obj)
{
    qCDebug(mLog) << "Load Palette..";

    QString paletteFilePath = QDir(obj->dataDir()).filePath(PFF_PALETTE_FILE);
    if (!obj->importPalette(paletteFilePath))
    {
        obj->loadDefaultPalette();
    }
    return true;
}

void FileManager::unzip(const QString& strZipFile, const QString& strUnzipTarget)
{
    // removes the previous directory first  - better approach
    removePFFTmpDirectory(strUnzipTarget);

    Status s = MiniZ::uncompressFolder(strZipFile, strUnzipTarget);
    Q_ASSERT(s.ok());

    mstrLastTempFolder = strUnzipTarget;
}

QList<ColourRef> FileManager::loadPaletteFile(QString strFilename)
{
    QFileInfo fileInfo(strFilename);
    if (!fileInfo.exists())
    {
        return QList<ColourRef>();
    }

    // TODO: Load Palette.
    return QList<ColourRef>();
}

Status FileManager::verifyObject(Object* obj)
{
    // check current layer.
    int curLayer = obj->data()->getCurrentLayer();
    int maxLayer = obj->getLayerCount();
    if (curLayer >= maxLayer)
    {
        obj->data()->setCurrentLayer(maxLayer - 1);
    }
    
    // Must have at least 1 camera layer
    std::vector<LayerCamera*> camLayers = obj->getLayersByType<LayerCamera>();
    if (camLayers.empty())
    {
        obj->addNewCameraLayer();
    }
    return Status::OK;
}
