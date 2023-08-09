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

#include "commandlineparser.h"

#include <QTextStream>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
const auto qEndl = Qt::endl;
#else
const auto qEndl = endl;
#endif

CommandLineParser::CommandLineParser() : mParser(), mInputPath(), mOutputPaths(), mCamera()
{
    mParser.setApplicationDescription(tr("Pencil2D is an animation/drawing software for Mac OS X, Windows, and Linux. "
                                         "It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics."));
    mParser.addHelpOption();
    mParser.addVersionOption();
    mParser.addPositionalArgument("input", tr("Path to the input pencil file."));

    QCommandLineOption exportOutOption(QStringList() << "o" << "export",
                                       tr("Render the file to <output_path>"),
                                       tr("output_path"));
    mParser.addOption(exportOutOption);

    // for backwards compatibility
    QCommandLineOption exportSeqOption(QStringList() << "export-sequence",
                                       tr("Render the file to <output_path>"),
                                       tr("output_path"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    exportSeqOption.setFlags(QCommandLineOption::HiddenFromHelp);
#endif
    mParser.addOption(exportSeqOption);

    QCommandLineOption cameraOption(QStringList() << "camera",
                                    tr("Name of the camera layer to use"),
                                    tr("layer_name"));
    mParser.addOption(cameraOption);

    QCommandLineOption widthOption(QStringList() << "width",
                                   tr("Width of the output frames"),
                                   tr("integer"));
    mParser.addOption(widthOption);

    QCommandLineOption heightOption(QStringList() << "height",
                                    tr("Height of the output frames"),
                                    tr("integer"));
    mParser.addOption(heightOption);

    QCommandLineOption startOption(QStringList() << "start",
                                   tr("The first frame you want to include in the exported movie"),
                                   tr("frame"));
    mParser.addOption(startOption);

    QCommandLineOption endOption(QStringList() << "end",
                                 tr("The last frame you want to include in the exported movie. "
                                    "Can also be last or last-sound to automatically use the last "
                                    "frame containing animation or sound, respectively"),
                                 tr("frame"));
    mParser.addOption(endOption);

    QCommandLineOption transparencyOption(QStringList() << "transparency",
                                          tr("Render transparency when possible"));
    mParser.addOption(transparencyOption);
}

void CommandLineParser::process(QStringList arguments)
{
    QTextStream out(stdout);
    QTextStream err(stderr);

    arguments.removeOne("-NSDocumentRevisionsDebugMode");

    mParser.process(arguments);

    QStringList posArgs = mParser.positionalArguments();
    if (!posArgs.isEmpty())
    {
        mInputPath = posArgs.at(0);
    }

    mOutputPaths << mParser.values("export") << mParser.values("export-sequence");

    if (!mParser.value("width").isEmpty())
    {
        bool ok = false;
        mWidth = mParser.value("width").toInt(&ok);
        if (!ok)
        {
            err << tr("Warning: width value %1 is not an integer, ignoring.").arg(mParser.value("width")) << qEndl;
            mWidth = -1;
        }
    }

    if (!mParser.value("height").isEmpty())
    {
        bool ok = false;
        mHeight = mParser.value("height").toInt(&ok);
        if (!ok)
        {
            err << tr("Warning: height value %1 is not an integer, ignoring.").arg(mParser.value("height")) << qEndl;
            mHeight = -1;
        }
    }

    if (!mParser.value("start").isEmpty())
    {
        bool ok = false;
        mStartFrame = mParser.value("start").toInt(&ok);
        if (!ok)
        {
            err << tr("Warning: start value %1 is not an integer, ignoring.").arg(mParser.value("start")) << qEndl;
            mStartFrame = 1;
        }
        if (mStartFrame < 1)
        {
            err << tr("Warning: start value must be at least 1, ignoring.") << qEndl;
            mStartFrame = 1;
        }
    }

    if (!mParser.value("end").isEmpty())
    {
        if (mParser.value("end") == "last")
        {
            mEndFrame = -1;
        }
        else if (mParser.value("end") == "last-sound")
        {
            mEndFrame = -2;
        }
        else
        {
            bool ok = false;
            mEndFrame = mParser.value("end").toInt(&ok);
            if (!ok)
            {
                err << tr("Warning: end value %1 is not an integer, last or last-sound, ignoring.").arg(mParser.value("end")) << qEndl;
                mEndFrame = -1;
            }
        }
        if (mEndFrame > -1 && mEndFrame < mStartFrame)
        {
            err << tr("Warning: end value %1 is smaller than start value %2, ignoring.").arg(mEndFrame).arg(mStartFrame) << qEndl;
            mEndFrame = mStartFrame;
        }
    }

    mTransparency = mParser.isSet("transparency");

    mCamera = mParser.value("camera");
}
