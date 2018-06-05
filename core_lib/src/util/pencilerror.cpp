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

#include "pencilerror.h"
#include <map>
#include <QObject>
#include <QSysInfo>
#include "pencildef.h"

DebugDetails::DebugDetails()
{
}

DebugDetails::~DebugDetails()
{
}

void DebugDetails::collect(const DebugDetails& d)
{
    for (const QString& s : d.mDetails)
    {
        mDetails.append("&nbsp;&nbsp;" + s);
    }
}

QString DebugDetails::str()
{
    appendSystemInfo();
    return mDetails.join("\n");
}

QString DebugDetails::html()
{
    appendSystemInfo();
    return mDetails.join("<br>");
}

DebugDetails& DebugDetails::operator<<(const QString& s)
{
    mDetails.append(s);
    return *this;
}

void DebugDetails::appendSystemInfo()
{
    if (mDetails.last() == "end")
        return;

#if QT_VERSION >= 0x050400
    mDetails << "System Info";
#if !defined(PENCIL2D_RELEASE)
    mDetails << "Pencil version: " APP_VERSION " (dev)";
#else
    mDetails << "Pencil version: " APP_VERSION " (stable)";
#endif
#if defined(GIT_EXISTS)
    mDetails << "Commit: " S__GIT_COMMIT_HASH;
#endif
    mDetails << "Build ABI: " + QSysInfo::buildAbi();
    mDetails << "Kernel: " + QSysInfo::kernelType() + ", " + QSysInfo::kernelVersion();
    mDetails << "Operating System: " + QSysInfo::prettyProductName();
    mDetails << "end";
#endif
}

Status::Status(ErrorCode code)
    : mCode(code)
{
}

Status::Status(Status::ErrorCode eCode, const DebugDetails& detailsList, QString title, QString description)
    : mCode(eCode)
    , mTitle(title)
    , mDescription(description)
    , mDetails(detailsList)
{
}

QString Status::msg()
{
    static std::map<ErrorCode, QString> msgMap =
    {
        // error messages.
        { OK,                    QObject::tr("Everything ok.") },
        { FAIL,                  QObject::tr("Ooops, Something went wrong.") },
        { FILE_NOT_FOUND,        QObject::tr("File doesn't exist.") },
        { ERROR_FILE_CANNOT_OPEN,    QObject::tr("Cannot open file.") },
        { ERROR_INVALID_XML_FILE,    QObject::tr("The file is not a valid xml document.") },
        { ERROR_INVALID_PENCIL_FILE, QObject::tr("The file is not valid pencil document.") },
    };

    auto it = msgMap.find(mCode);
    if (it == msgMap.end())
    {
        return msgMap[FAIL];
    }
    return msgMap[mCode];
}

bool Status::operator==(Status::ErrorCode code) const
{
    return (mCode == code);
}
