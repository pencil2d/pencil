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

#ifndef COMMANDLINEEXPORTER_H
#define COMMANDLINEEXPORTER_H

#include <QTextStream>

class LayerCamera;
class MainWindow2;

/**
 * Handles command line export jobs
 */
class CommandLineExporter : QObject
{
    Q_OBJECT

public:
    /**
     * Creates a new exporter instance.
     *
     * @param mainWindow The *main window* to be used by this *command line* exporter. Don't ask.
     */
    explicit CommandLineExporter(MainWindow2 *mainWindow);

    /**
     * Exports a Pencil2D file according to the specified options.
     *
     * @param inputPath Path of the file to export
     * @param outputPaths At least one export destination file path
     * @param camera Name of the camera layer to use, or an empty string to select one automatically
     * @param width Width to use for export, or a negative value to use the dimensions from the camera layer
     * @param height Height to use for export, or a negative value to use the dimensions from the camera layer
     * @param startFrame First frame to include in the export(s)
     * @param endFrame Last frame to include in the export(s) or -1 to use the last keyframe or -2 to use the last
     *                 keyframe including sound clips
     * @param transparency Whether to export with transparency
     * @return `true` if the export was successful
     */
    bool process(const QString &inputPath,
                 const QStringList &outputPaths,
                 const QString &camera,
                 int width,
                 int height,
                 int startFrame,
                 int endFrame,
                 bool transparency);

private:
    MainWindow2 *mMainWindow;
    QTextStream mOut;
    QTextStream mErr;
    void exportMovie(const QString &outputPath,
                     const LayerCamera *cameraLayer,
                     const QSize &exportSize,
                     int startFrame,
                     int endFrame,
                     bool transparency);
    void exportImageSequence(const QString &outputPath,
                             const QString &format,
                             const LayerCamera *cameraLayer,
                             const QSize &exportSize,
                             int startFrame,
                             int endFrame,
                             bool transparency);
};

#endif // COMMANDLINEEXPORTER_H
