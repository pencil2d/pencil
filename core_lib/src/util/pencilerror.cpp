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

#include "pencilerror.h"
#include <map>
#include <QSysInfo>
#include <QLocale>

DebugDetails::DebugDetails()
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
    if (mDetails.empty() || mDetails.last() == "end")
        return;

    mDetails << "\n[System Info]\n";
    QString version(APP_VERSION);
    if (version.startsWith("99.0.0")) {
        mDetails << "&nbsp;&nbsp;Pencil2D version: " APP_VERSION " (nightly)";
    } else if (version == "0.0.0.0") {
        mDetails << "&nbsp;&nbsp;Pencil2D version: " APP_VERSION " (dev)";
    } else {
        mDetails << "&nbsp;&nbsp;Pencil2D version: " APP_VERSION " (stable)";
    }

#if defined(GIT_EXISTS)
    mDetails << "&nbsp;&nbsp;Commit: " S__GIT_COMMIT_HASH;
#endif
    mDetails << "&nbsp;&nbsp;Build ABI: " + QSysInfo::buildAbi();
    mDetails << "&nbsp;&nbsp;Kernel: " + QSysInfo::kernelType() + ", " + QSysInfo::kernelVersion();
    mDetails << "&nbsp;&nbsp;Operating System: " + QSysInfo::prettyProductName();
    mDetails << "&nbsp;&nbsp;Language: " + QLocale::system().name();
}

Status::Status(ErrorCode code)
    : mCode(code)
{
}

Status::Status(const ErrorCode code, const QString& description)
    : mCode(code)
    , mDescription(description)
{
}

Status::Status(Status::ErrorCode eCode, const DebugDetails& detailsList)
    : mCode(eCode)
    , mDetails(detailsList)
{
}

Status::Status(Status::ErrorCode eCode, const DebugDetails& detailsList, QString title, QString description)
    : mCode(eCode)
    , mTitle(title)
    , mDescription(description)
    , mDetails(detailsList)
{
}


QString Status::msg() const
{
    static std::map<ErrorCode, QString> msgMap =
    {
        // error messages.
        { OK,                    tr("Everything ok.") },
        { FAIL,                  tr("Ooops, Something went wrong.") },
        { FILE_NOT_FOUND,        tr("File doesn't exist.") },
        { ERROR_FILE_CANNOT_OPEN,    tr("Cannot open file.") },
        { ERROR_INVALID_XML_FILE,    tr("The file is not a valid xml document.") },
        { ERROR_INVALID_PENCIL_FILE, tr("The file is not valid pencil document.") },
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

bool Status::operator!=(Status::ErrorCode code) const
{
    return (mCode != code);
}
