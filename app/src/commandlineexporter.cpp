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

#include <QFileInfo>
#include <QTextStream>

#include "editor.h"
#include "fileformat.h"
#include "layercamera.h"
#include "layermanager.h"
#include "mainwindow2.h"
#include "movieexporter.h"
#include "object.h"
#include "playbackmanager.h"

#include "commandlineexporter.h"

CommandLineExporter::CommandLineExporter(MainWindow2 *mainWindow) :
    mMainWindow(mainWindow),
    mOut(stdout, QIODevice::WriteOnly),
    mErr(stderr, QIODevice::WriteOnly)
{

}

bool CommandLineExporter::process(const QString &inputPath,
                                  const QStringList &outputPaths,
                                  const QString &camera,
                                  int width,
                                  int height,
                                  int startFrame,
                                  int endFrame,
                                  bool transparency)
{
    LayerManager *layerManager = mMainWindow->mEditor->layers();

    if (inputPath.isEmpty())
    {
        // Need a file to export
        mErr << tr("Error: No input file specified.") << endl;
        return false;
    }
    QFileInfo inputFileInfo(inputPath);
    if (!inputFileInfo.exists())
    {
        mErr << tr("Error: the input file at '%1' does not exist", "Command line error").arg(inputPath) << endl;
        return false;
    }
    if (!inputFileInfo.isFile())
    {
        mErr << tr("Error: the input path '%1' is not a file", "Command line error").arg(inputPath) << endl;
        return false;
    }
    Q_ASSERT(!outputPaths.empty());

    mMainWindow->openFile(inputPath);

    LayerCamera *cameraLayer = nullptr;
    if (!camera.isEmpty())
    {
        cameraLayer = qobject_cast<LayerCamera*>(layerManager->findLayerByName(camera, Layer::CAMERA));
        if (cameraLayer == nullptr)
        {
            mErr << tr("Warning: the specified camera layer %1 was not found, ignoring.").arg(camera) << endl;
        }
    }
    if (cameraLayer == nullptr)
    {
        cameraLayer = qobject_cast<LayerCamera*>(layerManager->getLastCameraLayer());
        Q_ASSERT(cameraLayer);
    }

    if (width < 0)
    {
        width = cameraLayer->getViewRect().width();
    }
    if (height < 0)
    {
        height = cameraLayer->getViewRect().height();
    }
    QSize exportSize(width, height);

    Q_ASSERT(startFrame >= 1);
    if (endFrame < 0)
    {
        endFrame = layerManager->animationLength(endFrame < -1);
    }

    for (const QString& outputPath : outputPaths)
    {
        // Detect format
        QString format = detectFormatByFileNameExtension(outputPath);
        if (format.isNull())
        {
            mErr << tr("Warning: Output format is not specified or unsupported. Using PNG.", "Command line warning") << endl;
            format = "PNG";
        }

        if (isMovieFormat(format))
        {
            exportMovie(outputPath, cameraLayer, exportSize, startFrame, endFrame, transparency);
            continue;
        }

        exportImageSequence(outputPath, format, cameraLayer, exportSize, startFrame, endFrame, transparency);
    }

    return true;
}

void CommandLineExporter::exportMovie(const QString &outputPath,
                                      const LayerCamera *cameraLayer,
                                      const QSize &exportSize,
                                      int startFrame,
                                      int endFrame,
                                      bool transparency)
{
    if (transparency)
    {
        mErr << tr("Warning: Transparency is not currently supported in movie files", "Command line warning") << endl;
    }

    mOut << tr("Exporting movie...", "Command line task progress") << endl;

    ExportMovieDesc desc;
    desc.strFileName = outputPath;
    desc.startFrame = startFrame;
    desc.endFrame = endFrame;
    desc.fps = mMainWindow->mEditor->playback()->fps();
    desc.exportSize = exportSize;
    desc.strCameraName = cameraLayer->name();

    MovieExporter ex;
    ex.run(mMainWindow->mEditor->object(), desc, [](float, float){}, [](float){}, [](const QString &){});
    mOut << tr("Done.", "Command line task done") << endl;
}

void CommandLineExporter::exportImageSequence(const QString &outputPath,
                                              const QString &format,
                                              const LayerCamera *cameraLayer,
                                              const QSize &exportSize,
                                              int startFrame,
                                              int endFrame,
                                              bool transparency)
{
    mOut << tr("Exporting image sequence...", "Command line task progress") << endl;
    mMainWindow->mEditor->object()->exportFrames(startFrame,
                                                 endFrame,
                                                 cameraLayer,
                                                 exportSize,
                                                 outputPath,
                                                 format,
                                                 transparency,
                                                 false,
                                                 "",
                                                 true,
                                                 nullptr,
                                                 0);
    mOut << tr("Done.", "Command line task done") << endl;
}
