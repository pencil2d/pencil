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

#include "filedialog.h"

#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>

#include "fileformat.h"
#include "pencildef.h"


QString FileDialog::getOpenFileName(QWidget* parent, FileType fileType, const QString& caption)
{
    QString strInitialFilePath = getLastOpenPath(fileType);
    QString strFilter = openFileFilters(fileType);
    QString strSelectedFilter = getFilterForFile(strFilter, strInitialFilePath);

    QString filePath = QFileDialog::getOpenFileName(parent,
                                                    caption.isEmpty() ? openDialogCaption(fileType) : caption,
                                                    strInitialFilePath,
                                                    strFilter,
                                                    strSelectedFilter.isNull() ? nullptr : &strSelectedFilter);
    if (!filePath.isEmpty())
    {
        setLastOpenPath(fileType, filePath);

        if (fileType == FileType::ANIMATION)
        {
            // When we open a project, change default export path for all filetypes
            setLastSavePaths(filePath);
        }
    }

    return filePath;
}

QStringList FileDialog::getOpenFileNames(QWidget* parent, FileType fileType, const QString& caption)
{
    QString strInitialFilePath = getLastOpenPath(fileType);
    QString strFilter = openFileFilters(fileType);
    QString strSelectedFilter = getFilterForFile(strFilter, strInitialFilePath);

    QStringList filePaths = QFileDialog::getOpenFileNames(parent,
                                                          caption.isEmpty() ? openDialogCaption(fileType) : caption,
                                                          strInitialFilePath,
                                                          strFilter,
                                                          strSelectedFilter.isNull() ? nullptr : &strSelectedFilter);
    if (!filePaths.isEmpty() && !filePaths.first().isEmpty())
    {
        setLastOpenPath(fileType, filePaths.first());
    }

    return filePaths;
}

void FileDialog::setLastSavePaths(const QString& filePath)
{
    QFileInfo filePathInfo(filePath);
    QDir projectPath = filePathInfo.absoluteDir();
    QString baseName = filePathInfo.baseName();
    QList<FileType> fileTypes = { FileType::IMAGE, FileType::IMAGE_SEQUENCE, FileType::GIF, FileType::MOVIE, FileType::SOUND, FileType::PALETTE };
    for (FileType& fileType : fileTypes)
    {
        setLastSavePath(fileType, projectPath.absoluteFilePath(defaultFileName(fileType, baseName)));
    }
}

QString FileDialog::getSaveFileName(QWidget* parent, FileType fileType, const QString& caption)
{
    QString strInitialFilePath = getLastSavePath(fileType);
    QString strFilter = saveFileFilters(fileType);
    QString strSelectedFilter = getFilterForFile(strFilter, strInitialFilePath);

    QString filePath = QFileDialog::getSaveFileName(parent,
                                                    caption.isEmpty() ? saveDialogCaption(fileType) : caption,
                                                    strInitialFilePath,
                                                    strFilter,
                                                    strSelectedFilter.isNull() ? nullptr : &strSelectedFilter);

    if (filePath.isEmpty()) { return QString(); }

    if (!hasValidSuffix(strFilter, filePath))
    {
        filePath += getDefaultExtensionByFileType(fileType);
    }

    if (fileType == FileType::ANIMATION)
    {
        // When we save a new project, change default path for all other filetypes
        setLastSavePaths(filePath);
    }

    setLastSavePath(fileType, filePath);

    return filePath;
}

QString FileDialog::getDefaultExtensionByFileType(const FileType fileType)
{
    switch (fileType)
    {
    case FileType::ANIMATION: return PFF_DEFAULT_PROJECT_EXT;
    case FileType::IMAGE: return PFF_DEFAULT_IMAGE_EXT;
    case FileType::IMAGE_SEQUENCE: return PFF_DEFAULT_IMAGE_SEQ_EXT;
    case FileType::GIF: return PFF_DEFAULT_ANIMATED_EXT;
    case FileType::ANIMATED_IMAGE: return PFF_DEFAULT_ANIMATED_EXT;
    case FileType::PALETTE: return PFF_DEFAULT_PALETTE_EXT;
    case FileType::MOVIE: return PFF_DEFAULT_MOVIE_EXT;
    case FileType::SOUND: return PFF_DEFAULT_SOUND_EXT;
    default:
        Q_UNREACHABLE();
    }
}

QString FileDialog::getLastOpenPath(FileType fileType)
{
    QSettings setting(PENCIL2D, PENCIL2D);
    setting.beginGroup("LastOpenPath");
    return setting.value(toSettingKey(fileType), QDir::homePath()).toString();
}

void FileDialog::setLastOpenPath(FileType fileType, const QString& openPath)
{
    QSettings setting(PENCIL2D, PENCIL2D);
    setting.beginGroup("LastOpenPath");

    setting.setValue(toSettingKey(fileType), QFileInfo(openPath).absolutePath());
}

QString FileDialog::getLastSavePath(FileType fileType)
{
    QSettings setting(PENCIL2D, PENCIL2D);
    setting.beginGroup("LastSavePath");

    return setting.value(toSettingKey(fileType),
                         QDir::homePath() + "/" + defaultFileName(fileType)).toString();
}

void FileDialog::setLastSavePath(FileType fileType, const QString& savePath)
{
    QSettings setting(PENCIL2D, PENCIL2D);
    setting.beginGroup("LastSavePath");

    setting.setValue(toSettingKey(fileType), savePath);
}

QString FileDialog::openDialogCaption(FileType fileType)
{
    switch (fileType)
    {
    case FileType::ANIMATION: return tr("Open animation");
    case FileType::IMAGE: return tr("Import image");
    case FileType::IMAGE_SEQUENCE: return tr("Import image sequence");
    case FileType::GIF: return tr("Import Animated GIF");
    case FileType::ANIMATED_IMAGE: return tr("Import animated image");
    case FileType::MOVIE: return tr("Import movie");
    case FileType::SOUND: return tr("Import sound");
    case FileType::PALETTE: return tr("Open palette");
    }
    return "";
}

QString FileDialog::saveDialogCaption(FileType fileType)
{
    switch (fileType)
    {
    case FileType::ANIMATION: return tr("Save animation");
    case FileType::IMAGE: return tr("Export image");
    case FileType::IMAGE_SEQUENCE: return tr("Export image sequence");
    case FileType::GIF: return tr("Export Animated GIF");
    case FileType::ANIMATED_IMAGE: return tr("Export animated image");
    case FileType::MOVIE: return tr("Export movie");
    case FileType::SOUND: return "";
    case FileType::PALETTE: return tr("Export palette");
    }
    return "";
}

QString FileDialog::openFileFilters(FileType fileType)
{
    switch (fileType)
    {
    case FileType::ANIMATION: return PFF_OPEN_PROJECT_EXT_FILTER;
    case FileType::IMAGE: return PFF_IMAGE_FILTER;
    case FileType::IMAGE_SEQUENCE: return PFF_IMAGE_SEQ_FILTER;
    case FileType::GIF: return PFF_GIF_EXT_FILTER;
    case FileType::ANIMATED_IMAGE: return PFF_ANIMATED_IMAGE_EXT_FILTER;
    case FileType::MOVIE: return PFF_MOVIE_EXT;
    case FileType::SOUND: return PFF_SOUND_EXT_FILTER;
    case FileType::PALETTE: return PFF_PALETTE_EXT_FILTER;
    }
    return "";
}

QString FileDialog::saveFileFilters(FileType fileType)
{
    switch (fileType)
    {
    case FileType::ANIMATION: return PFF_SAVE_PROJECT_EXT_FILTER;
    case FileType::IMAGE: return "";
    case FileType::IMAGE_SEQUENCE: return "";
    case FileType::GIF: return QString("%1 (*.gif)").arg(tr("Animated GIF"));
    case FileType::ANIMATED_IMAGE: return "";
    case FileType::MOVIE: return "MP4 (*.mp4);; AVI (*.avi);; WebM (*.webm);; APNG (*.apng)";
    case FileType::SOUND: return "";
    case FileType::PALETTE: return PFF_PALETTE_EXT_FILTER;
    }
    return "";
}

bool FileDialog::hasValidSuffix(const QString& filters, const QString& filePath)
{
    QString fileName = QFileInfo(filePath).fileName();
    for (const QString& filter : filters.split(";;"))
    {
        int start = filter.indexOf("(") + 1;
        int end = filter.indexOf(")");
        Q_ASSERT(start >= 1 && end >= 0);

        if (QDir::match(filter.mid(start, end - start), fileName)) {
            return true;
        }
    }
    return false;
}

QString FileDialog::getFilterForFile(const QString& filters, QString filePath)
{
    if (!filePath.contains("."))
    {
        return QString();
    }
    QString fileExt = filePath.remove(0, filePath.lastIndexOf(".")).prepend("*");

    QStringList filtersSplit = filters.split(";;");
    for (const QString& filter : filtersSplit)
    {
        int start = filter.indexOf("(");
        int end = filter.indexOf(")");
        if (start < 0 || end < 0)
        {
            continue;
        }
        start++;
        QStringList filterExts = filter.mid(start, end - start).split(" ");
        if (filterExts.contains(fileExt))
        {
            return filter.trimmed();
        }
    }

    return QString();
}

QString FileDialog::defaultFileName(FileType fileType, QString baseName)
{
    QString defaultName = tr("untitled");
    if (!baseName.isEmpty())
    {
        defaultName = baseName;
    }
    else if (fileType == FileType::ANIMATION)
    {
        defaultName = tr("MyAnimation");
    }

    return defaultName.append(getDefaultExtensionByFileType(fileType));
}

QString FileDialog::toSettingKey(FileType fileType)
{
    switch (fileType)
    {
    case FileType::ANIMATION: return "Animation";
    case FileType::IMAGE: return "Image";
    case FileType::IMAGE_SEQUENCE: return "ImageSequence";
    case FileType::GIF: return "Animated GIF";
    case FileType::ANIMATED_IMAGE: return "Animated Image";
    case FileType::MOVIE: return "Movie";
    case FileType::SOUND: return "Sound";
    case FileType::PALETTE: return "Palette";
    }
    return "";
}
