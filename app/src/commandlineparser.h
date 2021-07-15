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

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QCommandLineParser>

class CommandLineParser : QObject
{
    Q_OBJECT

public:
    explicit CommandLineParser();

    void process(QStringList arguments);

    QString inputPath() const { return mInputPath; }
    QStringList outputPaths() const { return mOutputPaths; }
    QString camera() const { return mCamera; }
    int width() const { return mWidth; }
    int height() const { return mHeight; }
    int startFrame() const { return mStartFrame; }
    int endFrame() const { return mEndFrame; }
    bool transparency() const { return mTransparency; }

private:
    QCommandLineParser mParser;

    QString mInputPath;
    QStringList mOutputPaths;
    QString mCamera;
    int mWidth = -1;
    int mHeight = -1;
    int mStartFrame = 1;
    int mEndFrame = -1;
    bool mTransparency = false;
};

#endif // COMMANDLINEPARSER_H
