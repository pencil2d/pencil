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

#ifndef PENCILERROR_H
#define PENCILERROR_H

#include <QString>
#include <QStringList>

class Status
{
public:
    enum ErrorCode
    {
        OK = 0,
        SAFE,
        FAIL,
		CANCELED,
        FILE_NOT_FOUND,
        NOT_SUPPORTED,
        INVALID_ARGUMENT,
        NOT_IMPLEMENTED_YET,

        // for Object load/save
        ERROR_FILE_CANNOT_OPEN,
        ERROR_INVALID_XML_FILE,
        ERROR_INVALID_PENCIL_FILE,
        ERROR_MINIZ_FAIL,

        // General
        ERROR_INVALID_LAYER_TYPE,
        ERROR_INVALID_FRAME_NUMBER,
        ERROR_LOAD_IMAGE_FAIL,

        // Sound
        ERROR_LOAD_SOUND_FILE,

		// Export
		ERROR_FFMPEG_NOT_FOUND,

        // Layer
        ERROR_NEED_AT_LEAST_ONE_CAMERA_LAYER,
    };


    Status() {}
    Status( ErrorCode eCode, QStringList detailsList = QStringList(), QString title = QString(), QString description = QString() );

    ErrorCode   code() { return mCode; }
    bool        ok() const { return ( mCode == OK ) || ( mCode == SAFE ); }
    QString     msg();
    QString     title() { return !mTitle.isEmpty() ? mTitle : msg(); }
    QString     description() { return mDescription; }
    QString     details();
    QStringList detailsList() { return mDetails; }

    void setTitle( QString title ) { mTitle = title; }
    void setDescription( QString description ) { mDescription = description; }
    void setDetailsList( QStringList detailsList ) { mDetails = detailsList; }

    bool operator==( ErrorCode code ) const;

private:
    ErrorCode mCode = OK;
    QString mTitle, mDescription;
    QStringList mDetails;
};

#ifndef STATUS_CHECK 
#define STATUS_CHECK( x )\
	{ Status st = (x); if ( !st.ok() ) { return st; } }
#endif
#endif // PENCILERROR_H
